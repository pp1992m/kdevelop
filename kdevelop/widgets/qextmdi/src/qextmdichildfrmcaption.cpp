//----------------------------------------------------------------------------
//    filename             : qextmdichildfrmcaption.cpp
//----------------------------------------------------------------------------
//    Project              : Qt MDI extension
//
//    begin                : 07/1999       by Szymon Stefanek as part of kvirc
//                                         (an IRC application)
//    changes              : 09/1999       by Falk Brettschneider to create an
//                           - 06/2000     stand-alone Qt extension set of
//                                         classes and a Qt-based library
//
//    copyright            : (C) 1999-2000 by Szymon Stefanek (stefanek@tin.it)
//                                         and
//                                         Falk Brettschneider
//    email                :  gigafalk@yahoo.com (Falk Brettschneider)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Library General Public License as
//    published by the Free Software Foundation; either version 2 of the
//    License, or (at your option) any later version.
//
//----------------------------------------------------------------------------

#include <qpainter.h>
#include <qapplication.h>

#include "qextmdidefines.h"
#include "qextmdichildfrmcaption.h"
#include "qextmdichildfrm.h"
#include "qextmdichildarea.h"
#include "qextmdimainfrm.h"
#include <iostream.h>
//////////////////////////////////////////////////////////////////////////////
// Class   : QextMdiChildFrmCaption
// Purpose : An MDI label that draws the title
//
//
//////////////////////////////////////////////////////////////////////////////

//============== QextMdiChildFrmCaption =============//

QextMdiChildFrmCaption::QextMdiChildFrmCaption(QextMdiChildFrm *parent)
:QWidget(parent, "qextmdi_childfrmcaption")
{
   m_szCaption    = tr("Unnamed");
   m_bActive      = FALSE;
   m_bCanMove     = FALSE;
   m_pParent      = parent;
   setBackgroundMode(NoBackground);
   setFocusPolicy(NoFocus);
}

//============== ~QextMdiChildFrmCaption =============//

QextMdiChildFrmCaption::~QextMdiChildFrmCaption()
{
}

//============= mousePressEvent ==============//

void QextMdiChildFrmCaption::mousePressEvent(QMouseEvent *e)
{
   if ( e->button() == LeftButton) {
      setMouseTracking(FALSE);
      if (QextMdiMainFrm::frameDecorOfAttachedViews() != QextMdi::Win95Look)
         QApplication::setOverrideCursor(Qt::sizeAllCursor,TRUE);
      m_bCanMove = TRUE;
      m_offset = mapToParent( e->pos());
   }
}

//============= mouseReleaseEvent ============//

void QextMdiChildFrmCaption::mouseReleaseEvent(QMouseEvent *e)
{
   if ( e->button() == LeftButton) {
      if (QextMdiMainFrm::frameDecorOfAttachedViews() != QextMdi::Win95Look)
         QApplication::restoreOverrideCursor();
      releaseMouse();
      m_bCanMove = FALSE;
   }
}

//============== mouseMoveEvent =============//
void QextMdiChildFrmCaption::mouseMoveEvent(QMouseEvent *e)
{
   if ( !m_bCanMove )
      return;
   QPoint relMousePosInChildArea = m_pParent->m_pManager->mapFromGlobal( e->globalPos() );

   // mouse out of child area? stop child frame dragging
   if ( !m_pParent->m_pManager->rect().contains( relMousePosInChildArea)) {
      if ( relMousePosInChildArea.x() < 0)
         relMousePosInChildArea.rx() = 0;
      if ( relMousePosInChildArea.y() < 0)
         relMousePosInChildArea.ry() = 0;
      if ( relMousePosInChildArea.x() > m_pParent->m_pManager->width())
         relMousePosInChildArea.rx() = m_pParent->m_pManager->width();
      if ( relMousePosInChildArea.y() > m_pParent->m_pManager->height())
         relMousePosInChildArea.ry() = m_pParent->m_pManager->height();
   }
   QPoint mousePosInChildArea = relMousePosInChildArea - m_offset;

   // set new child frame position
   parentWidget()->move( mousePosInChildArea);
}

//=============== setActive ===============//

void QextMdiChildFrmCaption::setActive(bool bActive)
{
   if( m_bActive == bActive)
      return;
   
   //    Ensure the icon's pixmap has the correct bg color
   m_pParent->m_pWinIcon->setBackgroundColor(bActive
   ? m_pParent->m_pManager->m_captionActiveBackColor
   : m_pParent->m_pManager->m_captionInactiveBackColor);
   m_pParent->m_pUnixIcon->setBackgroundColor(bActive
   ? m_pParent->m_pManager->m_captionActiveBackColor
   : m_pParent->m_pManager->m_captionInactiveBackColor);

   m_bActive = bActive;
   repaint( FALSE);
}

//=============== setCaption ===============//

void QextMdiChildFrmCaption::setCaption(const QString& text)
{
   m_szCaption = text;
   repaint(FALSE);
}

//============== heightHint ===============//

int QextMdiChildFrmCaption::heightHint()
{
   int hght=m_pParent->m_pManager->m_captionFontLineSpacing+2;
   if (QextMdiMainFrm::frameDecorOfAttachedViews() == QextMdi::Win95Look) {
      if(hght<18)hght=18;
   }
   else if (QextMdiMainFrm::frameDecorOfAttachedViews() == QextMdi::KDE1Look) {
      if(hght<20)hght=20;
   }
   else {
      if(hght<14)hght=14;
   }
   return hght;
}

//=============== paintEvent ==============//

void QextMdiChildFrmCaption::paintEvent(QPaintEvent *)
{
   QPainter p(this);
   QRect r=rect();
   p.setFont(m_pParent->m_pManager->m_captionFont);
   if(m_bActive){
      p.fillRect(r,m_pParent->m_pManager->m_captionActiveBackColor);
      p.setPen(m_pParent->m_pManager->m_captionActiveForeColor);
   } else {
      p.fillRect(r,m_pParent->m_pManager->m_captionInactiveBackColor);
      p.setPen(m_pParent->m_pManager->m_captionInactiveForeColor);
   }
   //Shift the text after the icon
   if (QextMdiMainFrm::frameDecorOfAttachedViews() == QextMdi::Win95Look)
      r.setLeft(r.left()+19);
   else if (QextMdiMainFrm::frameDecorOfAttachedViews() == QextMdi::KDE1Look)
      r.setLeft(r.left()+22);
   else
      r.setLeft(r.left()+30);

   int captionWidthForText = width() - 5*heightHint();   // = width - width_for_buttons
   QString text = abbreviateText( m_szCaption, captionWidthForText);
   p.drawText( r, AlignVCenter|AlignLeft|SingleLine, text);
   
}

//=============== abbreviateText ===============//

QString QextMdiChildFrmCaption::abbreviateText(QString origStr, int maxWidth)
{
   QFontMetrics fm = fontMetrics();
   int actualWidth = fm.width( origStr);

   int realLetterCount = origStr.length();
   int newLetterCount;
   if (actualWidth != 0) {
      newLetterCount = (maxWidth * realLetterCount) / actualWidth;
   }
   else {
      newLetterCount = realLetterCount; // should be 0 anyway
   }
   int w = maxWidth+1;
   QString s = origStr;
   while((w > maxWidth) && (newLetterCount >= 1)) {
      if( newLetterCount < realLetterCount) {
         if(newLetterCount > 3)
            s = origStr.left( newLetterCount/2) + "..." + origStr.right( newLetterCount/2);
         else {
            if(newLetterCount > 1)
               s = origStr.left( newLetterCount) + "..";
            else
               s = origStr.left(1);
         }
      }
      QFontMetrics fm = fontMetrics();
      w = fm.width(s);
      newLetterCount--;
   }

   return s;
}

//============= mouseDoubleClickEvent ===========//

void QextMdiChildFrmCaption::mouseDoubleClickEvent(QMouseEvent *)
{
   m_pParent->maximizePressed();
}

void QextMdiChildFrmCaption::slot_moveViaSystemMenu()
{
   setMouseTracking(TRUE);
   grabMouse();
   if (QextMdiMainFrm::frameDecorOfAttachedViews() != QextMdi::Win95Look)
      QApplication::setOverrideCursor(Qt::sizeAllCursor,TRUE);
   m_bCanMove = TRUE;
   m_offset = mapFromGlobal( QCursor::pos());
}

#ifndef NO_INCLUDE_MOCFILES
#include "qextmdichildfrmcaption.moc"
#endif
