/*
 * This file is part of KDevelop
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "debug.h"

// TODO: ecm_qt_declare_logging_category only supports one category
// so generate two separate files with a wrapper debug.h, or have code include explicit matching header?
const QtMsgType defaultMsgType = QtInfoMsg;
Q_LOGGING_CATEGORY(PROJECT, "kdevplatform.project", defaultMsgType)
Q_LOGGING_CATEGORY(FILEMANAGER, "kdevplatform.filemanager", defaultMsgType)
