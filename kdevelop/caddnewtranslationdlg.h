/***************************************************************************
                          caddnewtranslationdlg.h  -  description                              
                             -------------------                                         
    begin                : Thu Apr 8 1999                                           
    copyright            : (C) 1999 by Sandy Meier                         
    email                : smeier@rz.uni-potsdam.de                                     
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#ifndef CADDNEWTRANSLATIONDLG_H
#define CADDNEWTRANSLATIONDLG_H

#include <qdialog.h>
#include <qstringlist.h>

class QPushButton;
class QComboBox;
class CProject;

/**
 *@author Sandy Meier
 */

class CAddNewTranslationDlg : public QDialog  {
  Q_OBJECT
public: 
  CAddNewTranslationDlg(QWidget *parent=0, const char *name=0,CProject* p_prj=0);
  ~CAddNewTranslationDlg();
  QString getLangFile();
protected:
  QPushButton* ok_button;
  QPushButton* cancel_button;
  QComboBox* lang_combo;
  QStringList lang_list;
  QString langfile;
  CProject* prj;

protected slots:
void slotOkClicked();

};

#endif
