// ============================================================================
// File: widgetwithbackground.cpp
// Subsystem: eLynxSDR led
// Description: Subsystem implementation and interface guidance for widgetwithbackground.cpp.
// ============================================================================

/***************************************************************************
 *   Copyright (C) 2006-2008 by Tomasz Ziobrowski                          *
 *   http://www.3electrons.com                                             *
 *   e-mail: t.ziobrowski@3electrons.com                                   *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui>
#include "widgetwithbackground.h"

// Constructor: Initializes WidgetWithBackground subsystem components, default parameters, and state.
WidgetWithBackground::WidgetWithBackground(QWidget * parent) : QWidget(parent)
{
  m_pixmap = new QPixmap(size());
  m_modified = false;
}

// Destructor: Cleans up WidgetWithBackground resources, timers, and allocated memory.
WidgetWithBackground::~WidgetWithBackground()
{
   if (m_pixmap)
     {
	delete m_pixmap;
	m_pixmap = NULL;
     }
}

// Executes draw background routine.
void WidgetWithBackground::drawBackground()
{
  if (m_pixmap->size() != size() || m_modified )
    {
	delete m_pixmap;
	m_pixmap = new QPixmap(size());
	m_modified=true; // by wiadomo bylo e jest przemalowywane tlo
	repaintBackground();
	m_modified=false;
    }
    QPainter painter(this);
    painter.drawPixmap(0,0,*m_pixmap);
}

// Updates and synchronizes update with background data stream.
void WidgetWithBackground::updateWithBackground()
{
  m_modified=true;
  update();
}

// Executes do repaint background routine.
bool WidgetWithBackground::doRepaintBackground()
{
  return m_modified;
}

// Executes repaint background routine.
void WidgetWithBackground::repaintBackground()
{
  m_pixmap->fill(QColor(0,0,0,0));
  QPainter painter(m_pixmap);
  paintBackground(painter);
}
