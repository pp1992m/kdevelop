/***************************************************************************
 *   Copyright (C) 2003 by Mario Scalas                                    *
 *   mario.scalas@libero.it                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qtextstream.h>

#include "cvsentry.h"

///////////////////////////////////////////////////////////////////////////////
// Static
///////////////////////////////////////////////////////////////////////////////

const QString CVSEntry::invalidMarker = "<Invalid entry>";
const QString CVSEntry::directoryMarker = "D";
const QString CVSEntry::fileMarker = "";
const QString CVSEntry::entrySeparator = "/";

///////////////////////////////////////////////////////////////////////////////
// class CVSEntry
///////////////////////////////////////////////////////////////////////////////

CVSEntry::CVSEntry()
{
    clean();
}

///////////////////////////////////////////////////////////////////////////////

CVSEntry::CVSEntry( const QString &aLine )
{
    parse( aLine, *this );
}

///////////////////////////////////////////////////////////////////////////////

void CVSEntry::clean()
{
    m_type = invalidEntry;
}

///////////////////////////////////////////////////////////////////////////////

CVSEntry::EntryType CVSEntry::type() const
{
    return m_type;
}

///////////////////////////////////////////////////////////////////////////////

void CVSEntry::parse( const QString &aLine, CVSEntry &entry )
{
    entry.clean();

    entry.m_fields = QStringList::split( "/", aLine );

    if (aLine.startsWith( entrySeparator )) // Is a file?
    {
        entry.m_type = fileEntry; // Is a file
    }
    else if (aLine.startsWith( directoryMarker )) // Must be a directory then
    {
        entry.m_type = directoryEntry; // Is a directory
        entry.m_fields.pop_front(); // QStringList::split() fills and empty item in head
    }
    else // What the hell is this? >:-)
    {
        entry.m_type = invalidEntry;
    }
}

///////////////////////////////////////////////////////////////////////////////

QString CVSEntry::fileName() const
{
    if (type() != invalidEntry && m_fields.count() >= 1)
        return m_fields[0];
    else
        return QString::null;
}

///////////////////////////////////////////////////////////////////////////////

QString CVSEntry::revision() const
{
    if (type() != invalidEntry && m_fields.count() >= 2)
        return m_fields[1];
    else
        return QString::null;
}

///////////////////////////////////////////////////////////////////////////////

QString CVSEntry::timeStamp() const
{
    if (type() != invalidEntry && m_fields.count() >= 3)
        return m_fields[2];
    else
        return QString::null;
}

///////////////////////////////////////////////////////////////////////////////

QString CVSEntry::options() const
{
    if (type() != invalidEntry && m_fields.count() >= 4)
        return m_fields[3];
    else
        return QString::null;
}

///////////////////////////////////////////////////////////////////////////////

QString CVSEntry::tag() const
{
    if (type() != invalidEntry && m_fields.count() >= 5)
        return m_fields[4];
    else
        return QString::null;
}

///////////////////////////////////////////////////////////////////////////////

VCSFileInfo CVSEntry::toVCSFileInfo() const
{
    VCSFileInfo::FileState fileState = VCSFileInfo::Unknown;
    if (isDirectory())
        fileState = VCSFileInfo::Directory;

    return VCSFileInfo( fileName(), revision(), QString::null, fileState );
}
