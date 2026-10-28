// ============================================================================
// File: led.h
// Subsystem: eLynxSDR led
// Description: Subsystem implementation and interface guidance for led.h.
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

#ifndef QLED_H
#define QLED_H
#include <QColor> 
#include "widgetwithbackground.h"

   /**
   * Klasa reprezentujca diod w dowolnym kolorze jako dwustabilny element wskazujcy 
   */
   class Led : public WidgetWithBackground
   {
     Q_OBJECT 
     Q_PROPERTY( bool checked READ isChecked 	WRITE setChecked)
     Q_PROPERTY( QColor color READ color 	WRITE setColor) 
     
     
     public:  
     
     // Constructor: Initializes Led subsystem components, default parameters, and state.
     Led(QWidget *parent = 0);
     virtual ~Led() {};
     
     /** Zdarzenie obsugujce malowanie kontrolki */ 
     // Executes paint event routine.
     void paintEvent(QPaintEvent * event);
	 // Executes mouse press event routine.
	 void mousePressEvent(QMouseEvent *ev);
     // Retrieves current hecked state.
     bool isChecked () const ; 
     
     
     // Executes color routine.
     QColor color() const; 
     // Sets color value and updates internal subsystem configuration.
     void setColor(QColor); 
     
     public slots: 
     
     // Sets checked value and updates internal subsystem configuration.
     void setChecked(bool); 
     
     signals: 
     
     // Executes check changed routine.
     void checkChanged(bool val); 
	 // Event/Signal handler for clicked.
	 void clicked();
     protected:
     
     /** Inicjuje ukad wsprzdnych paintera */
     // Initializes init coordinate system for Led.
     void initCoordinateSystem(QPainter & painter);
     
     /**
     * Maluje to kontrolki w tym przypadku pierwszy plan czyli odblask wiata kontrolki
     * @param painter Przestrze kontrolki 
     */
     // Executes paint background routine.
     void paintBackground(QPainter & painter);
    
     protected: 
     
     bool m_checked; 
     QColor m_color; 

   }; 
   
#endif // QLED_H 
