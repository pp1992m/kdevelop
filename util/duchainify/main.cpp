/***************************************************************************
 *   Copyright 2009 David Nolden <david.nolden.kdevelop@art-master.de>                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "main.h"

#include <shell/core.h>
#include <shell/shellextension.h>

#include <language/backgroundparser/parsejob.h>
#include <language/backgroundparser/backgroundparser.h>
#include <language/duchain/duchain.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/duchaindumper.h>
#include <language/duchain/dumpdotgraph.h>
#include <language/duchain/problem.h>

#include <interfaces/ilanguage.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/ilanguagecontroller.h>
#include <tests/autotestshell.h>
#include <tests/testcore.h>

#include <KDE/KApplication>
#include <KDE/KCmdLineArgs>
#include <KDE/KDebug>
#include <k4aboutdata.h>

#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QDirIterator>

#include <stdio.h>

bool verbose=false, warnings=false;

using namespace KDevelop;

void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);

    switch (type) {
        case QtDebugMsg:
            if(verbose)
                std::cerr << qPrintable(msg) << std::endl;
            break;
        case QtWarningMsg:
            if(warnings)
                std::cerr << qPrintable(msg) << std::endl;
            break;
        case QtCriticalMsg:
            std::cerr << qPrintable(msg) << std::endl;
            break;
        case QtFatalMsg:
            std::cerr << qPrintable(msg) << std::endl;
            abort();
    }
}


Manager::Manager(KCmdLineArgs* args) : m_total(0), m_args(args), m_allFilesAdded(0)
{
}

void Manager::init()
{
    KUrl::List includes;

    if(m_args->count() == 0) {
        std::cerr << "Need file or directory to duchainify" << std::endl;
        QCoreApplication::exit(1);
    }

    uint features = TopDUContext::VisibleDeclarationsAndContexts;
    if(m_args->isSet("features"))
    {
        QString featuresStr = m_args->getOption("features");
        if(featuresStr == "visible-declarations")
        {
            features = TopDUContext::VisibleDeclarationsAndContexts;
        }
        else if(featuresStr == "all-declarations")
        {
            features = TopDUContext::AllDeclarationsAndContexts;
        }
        else if(featuresStr == "all-declarations-and-uses")
        {
            features = TopDUContext::AllDeclarationsContextsAndUses;
        }
        else if(featuresStr == "all-declarations-and-uses-and-AST")
        {
            features = TopDUContext::AllDeclarationsContextsAndUses | TopDUContext::AST;
        }
        else if(featuresStr == "empty")
        {
            features = TopDUContext::Empty;
        }
        else if(featuresStr == "simplified-visible-declarations")
        {
            features = TopDUContext::SimplifiedVisibleDeclarationsAndContexts;
        }
        else{
            std::cerr << "Wrong feature-string given\n";
            QCoreApplication::exit(2);
        }
    }
    if(m_args->isSet("force-update"))
        features |= TopDUContext::ForceUpdate;
    if(m_args->isSet("force-update-recursive"))
        features |= TopDUContext::ForceUpdateRecursive;
    
    if(m_args->isSet("threads"))
    {
        bool ok = false;
        int count = m_args->getOption("threads").toInt(&ok);
        ICore::self()->languageController()->backgroundParser()->setThreadCount(count);
        if(!ok) {
            std::cerr << "bad thread count\n";
            QCoreApplication::exit(3);
        }
    }

    // quit when everything is done
    // background parser emits hideProgress() signal in two situations:
    // when everything is done and when bgparser is suspended
    // later doesn't happen in duchain, so just rely on hideProgress()
    // and quit when it's emitted
    connect(ICore::self()->languageController()->backgroundParser(), SIGNAL(hideProgress(KDevelop::IStatus*)), this, SLOT(finish()));

    for(int i=0; i<m_args->count(); i++)
    {
        addToBackgroundParser(m_args->arg(i), (TopDUContext::Features)features);
    }
    m_allFilesAdded = 1;

    if ( m_total ) {
        std::cerr << "Added " << m_total << " files to the background parser" << std::endl;
        const int threads = ICore::self()->languageController()->backgroundParser()->threadCount();
        std::cerr << "parsing with " << threads << " threads" << std::endl;
        ICore::self()->languageController()->backgroundParser()->parseDocuments();
    } else {
        std::cerr << "no files added to the background parser" << std::endl;
        QCoreApplication::exit(0);
    }
}

void Manager::updateReady(IndexedString url, ReferencedTopDUContext topContext)
{
    kDebug() << "finished" << url.toUrl().toLocalFile() << "success: " << (bool)topContext;
    
    m_waiting.remove(url.toUrl());
    
    std::cerr << "processed " << (m_total - m_waiting.size()) << " out of " << m_total << std::endl;
    if (!topContext)
        return;

    DUChainDumper::Features features;
    if (m_args->isSet("dump-context")) {
        features |= DUChainDumper::DumpContext;
    }
    if (m_args->isSet("dump-errors")) {
        features |= DUChainDumper::DumpProblems;
    }

    DUChainReadLocker lock;
    DUChainDumper dumpChain(features);
    dumpChain.dump(topContext, m_args->getOption("dump-depth").toInt());

    if (m_args->isSet("dump-graph")) {
        DumpDotGraph dumpGraph;
        const QString dotOutput = dumpGraph.dotGraph(topContext);
        std::cout << qPrintable(dotOutput) << std::endl;
    }
}

void Manager::addToBackgroundParser(QString path, TopDUContext::Features features)
{
    QFileInfo info(path);
    
    if(info.isFile())
    {
        kDebug() << "adding file" << path;
        KUrl pathUrl(info.canonicalFilePath());
        
        m_waiting << pathUrl;
        ++m_total;
        
        KDevelop::DUChain::self()->updateContextForUrl(KDevelop::IndexedString(pathUrl), features, this);
        
    }else if(info.isDir())
    {
        QDirIterator contents(path);
        while(contents.hasNext()) {
            QString newPath = contents.next();
            if(!newPath.endsWith('.'))
                addToBackgroundParser(newPath, features);
        }
    }
}

QSet< KUrl > Manager::waiting()
{
    return m_waiting;
}

void Manager::finish()
{
    std::cerr << "ready" << std::endl;
    QApplication::quit();
}

using namespace KDevelop;
int main(int argc, char** argv)
{
    K4AboutData aboutData( "duchainify", 0, ki18n( "duchainify" ),
                          "1", ki18n("DUChain builder application"), K4AboutData::License_GPL,
                          ki18n( "(c) 2009 David Nolden" ), KLocalizedString(), "http://www.kdevelop.org" );
    KCmdLineArgs::init( argc, argv, &aboutData, KCmdLineArgs::CmdLineArgNone );
    KCmdLineOptions options;
    options.add("+path", ki18n("file or directory"));
    
    options.add("w").add("warnings", ki18n("Show warnings"));
    options.add("V").add("verbose", ki18n("Show warnings and debug output"));
    options.add("u").add("force-update", ki18n("Enforce an update of the top-contexts corresponding to the given files"));
    options.add("r").add("force-update-recursive", ki18n("Enforce an update of the top-contexts corresponding to the given files and all included files"));
    options.add("t").add("threads <count>", ki18n("Number of threads to use"));
    options.add("f").add("features <features>", ki18n("Features to build. Options: empty, simplified-visible-declarations, visible-declarations (default), all-declarations, all-declarations-and-uses, all-declarations-and-uses-and-AST"));
    options.add("dump-context", ki18n("Print complete Definition-Use Chain on successful parse"));
    options.add("dump-depth <depth>", ki18n("Number defining the maximum depth where declaration details are printed"));
    options.add("dump-graph", ki18n("Dump DUChain graph (in .dot format)"));
    options.add("d").add("dump-errors", ki18n("Print problems encountered during parsing"));
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    verbose = args->isSet("verbose");
    warnings = args->isSet("warnings");
    qInstallMessageHandler(messageOutput);

    KApplication app(false);

    AutoTestShell::init();
    TestCore::initialize(Core::NoUi, "duchainify");
    Manager manager(args);

    QTimer::singleShot(0, &manager, SLOT(init()));
    int ret = app.exec();

    TestCore::shutdown();

    return ret;
}
