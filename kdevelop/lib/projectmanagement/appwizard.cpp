/***************************************************************************
                          appwizard.cpp  -  description
                             -------------------
    begin                : Sat May 13 2000
    copyright            : (C) 2000 by Sandy Meier
    email                : smeier@kdevelop.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "appwizard.h"
#include "pluginloader.h"
#include <qfiledialog.h>
#include <iostream.h>
#include <kprocess.h>
#include <kdebug.h>
#include <qdatetime.h>
#include <pwd.h>
#include <unistd.h>
#include <qfile.h>
#include <qtextstream.h>
#include <ctoolclass.h>


AppWizard::AppWizard(QWidget* parent, const char* obj_name) : AppWizardBase(parent,obj_name,true){
  // some code from kbugreport by David Faure
  KConfig emailConf( QString::fromLatin1("emaildefaults") );
  emailConf.setGroup( QString::fromLatin1("UserInfo") );
  QString fromaddr = emailConf.readEntry( QString::fromLatin1("EmailAddress") );
  if (fromaddr.isEmpty()) {
     struct passwd *p;
     p = getpwuid(getuid());  
     fromaddr = QString::fromLatin1(p->pw_name) + "@";
  }
  QString name = emailConf.readEntry( QString::fromLatin1("FullName"));
  QString company = emailConf.readEntry( QString::fromLatin1("Organization"));
  
  email_edit->setText( fromaddr );
  author_edit->setText( name );
  company_edit->setText( company );
  
  // conntects
  m_project=0;
  
}
AppWizard::~AppWizard(){
}

void AppWizard::init(bool new_projectspace,ProjectSpace* projectspace,QString projectName,QString absProjectLocation){
  kdDebug(9000) << "enter AppWizard::init" << endl;
  m_new_projectspace = new_projectspace;
  m_projectspace = projectspace;
  m_projectName = projectName;
  m_absProjectLocation = absProjectLocation;

  // create the project
  m_project = PluginLoader::getNewProject(m_projecttype_name);
  m_project->setName(projectName);
  m_project->setAbsolutePath(absProjectLocation);
  QString relProjectPath = CToolClass::getRelativePath(projectspace->absolutePath(),
						       absProjectLocation);
  m_project->setRelativePath(relProjectPath);
 
  if(!m_new_projectspace){
    removePage(page(0)); // remove the name,license page
    removePage(page(0)); // remove the headerpage
  }
  
}

void AppWizard::accept(){
  kdDebug(9000) << "start generation" << endl;
  if(m_new_projectspace){ // only if a new one was selected
    m_projectspace->setAuthor(author_edit->text());
    m_projectspace->setEmail(email_edit->text());
    m_projectspace->setCompany(company_edit->text());
    m_projectspace->setVersion(version_edit->text());
    m_projectspace->generateDefaultFiles();
  }
  m_projectspace->addProject(m_project);
  generateDefaultFiles();
  if(!m_projectspace->writeXMLConfig()){
    kdDebug(9000) << "error in writing ConfigFile" << endl;
  }
  else{
    kdDebug(9000) << "ConfigFile successfully written" << endl;
  }
  QWizard::accept();
}

QString  AppWizard::getProjectSpaceName(){
  return m_project_space_name;
}
QString  AppWizard::getPreviewPicture(){
  return m_application_picture;
}

void AppWizard::generateDefaultFiles(){
  KShellProcess proc("/bin/sh");
  
  QString absProjectPath = m_project->getAbsolutePath();
  kdDebug(9000) << "creating directory: " << absProjectPath << endl;
  // create the directories
  proc << "mkdirhier";
  proc << absProjectPath;
  proc.start(KProcess::Block,KProcess::AllOutput);
  
  // untar/unzip the project
  proc.clearArguments();
  QString args = "xzvf " + m_project_template + " -C " + absProjectPath;
  cerr << "AppWizard::generateDefaultFiles():" << args;
  proc << "tar";
  proc << args;
  proc.start(KProcess::Block,KProcess::AllOutput);
}
void AppWizard::slotNewHeader(){
  header_multiedit->clear();
}
void AppWizard::slotLoadHeader(){
}


void AppWizard::setInfosInString(QString& text){
  QDate date;
  text.replace(QRegExp("|NAME|"),m_project->getName());
  text.replace(QRegExp("|NAMELITTLE|"),m_project->getName().lower());
  text.replace(QRegExp("|YEAR|"),QString::number(date.year()));
  text.replace(QRegExp("|EMAIL|"),email_edit->text());
  text.replace(QRegExp("|AUTHOR|"),author_edit->text());
  text.replace(QRegExp("|VERSION|"),version_edit->text());
}

void AppWizard::generateFile(QString abs_oldpos,QString abs_newpos){
  QFile file(abs_oldpos);
  if ( file.open(IO_ReadOnly) ) {    // file opened successfully
    QTextStream t( &file );        // use a text stream
    QString text = t.read();
    file.close();
    setInfosInString(text);
    // save
    file.setName(abs_newpos);
    if ( file.open(IO_WriteOnly) ){
      QTextStream tw( &file );
      tw << text;
      file.close();
    }
    else {
      cerr << "\nERROR! couldn't open file to write:" << abs_newpos;
    }
  }
  else {
    cerr << "\nERROR! couldn't open file to read:" << abs_oldpos;
  }
}

#include "appwizard.moc"
