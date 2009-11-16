/***************************************************************************
 *   This file is part of KDevelop                                         *
 *   Copyright 2009 Niko Sams <niko.sams@gmail.com>                        *
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

#include "ivariablecontroller.h"

#include "idebugsession.h"
#include "../../interfaces/icore.h"
#include "../../interfaces/idebugcontroller.h"
#include "../variable/variablecollection.h"

namespace KDevelop {

    
IVariableController::IVariableController(IDebugSession* parent)
    : QObject(parent)
{
}

VariableCollection* IVariableController::variableCollection()
{
    return ICore::self()->debugController()->variableCollection();
}

void IVariableController::handleEvent(IDebugSession::event_t event)
{
    switch (event) {
    case IDebugSession::program_exited:
    case IDebugSession::debugger_exited:
        // Remove all locals.
        foreach (Locals *l, variableCollection()->allLocals()) {
            l->deleteChildren();
            l->setHasMore(false);
        }

        for (int i=0; i < variableCollection()->watches()->childCount(); ++i) {
            Variable *var = dynamic_cast<Variable*>(variableCollection()->watches()->child(i));
            if (var) {
                var->setInScope(false);
            }
        }
        break;

    case IDebugSession::program_state_changed:
    case IDebugSession::thread_or_frame_changed:
        if (!(m_autoUpdate & UpdateLocals)) {
            foreach (Locals *l, variableCollection()->allLocals()) {
                l->setHasMore(true);
            }
        }
        if (m_autoUpdate != UpdateNone) {
            update();
        }
        break;

    default:
        break;
    }
}

void IVariableController::setAutoUpdate(QFlags<UpdateType> autoUpdate)
{
    IDebugSession::DebuggerState state = static_cast<IDebugSession*>(parent())->state();
    m_autoUpdate = autoUpdate;
    if (m_autoUpdate != UpdateNone && state == IDebugSession::PausedState) {
        update();
    }
}

QFlags<IVariableController::UpdateType> IVariableController::autoUpdate()
{
    return m_autoUpdate;
}

}

#include "ivariablecontroller.moc"
