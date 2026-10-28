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
     
     /**
      * @brief Constructs a new Led instance and initializes subsystem components and UI state.
      * @param parent Input parameter specifying parent (QWidget).
      */
     Led(QWidget *parent = 0);
     virtual ~Led() {};
     
     /** Zdarzenie obsugujce malowanie kontrolki */ 
     void paintEvent(QPaintEvent * event);
	 void mousePressEvent(QMouseEvent *ev);
     bool isChecked () const ; 
     
     
     /**
      * @brief Executes the color procedure for Led.
      * @return Output value of type QColor.
      */
     QColor color() const; 
     /**
      * @brief Sets the color parameter and updates internal state.
      * @param QColor Input parameter specifying QColor (QColor).
      */
     void setColor(QColor); 
     
     public slots: 
     
     /**
      * @brief Sets the checked parameter and updates internal state.
      * @param bool Input parameter specifying bool (bool).
      */
     void setChecked(bool); 
     
     signals: 
     
     /**
      * @brief Executes the check changed procedure for Led.
      * @param val Input parameter specifying val (bool).
      */
     void checkChanged(bool val); 
	 /**
	  * @brief Slot handler responding to clicked event.
	  */
	 void clicked();
     protected:
     
     /** Inicjuje ukad wsprzdnych paintera */
     void initCoordinateSystem(QPainter & painter);
     
     /**
     * Maluje to kontrolki w tym przypadku pierwszy plan czyli odblask wiata kontrolki
     * @param painter Przestrze kontrolki 
     */
     void paintBackground(QPainter & painter);
    
     protected: 
     
     bool m_checked; 
     QColor m_color; 

   }; 
   
#endif // QLED_H 
