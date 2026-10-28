// ============================================================================
// File: widgetwithbackground.h
// Subsystem: eLynxSDR led
// Description: Subsystem implementation and interface guidance for widgetwithbackground.h.
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

#ifndef WIDGETWITHBACKGROUND_H
#define WIDGETWITHBACKGROUND_H

#include <QWidget>

class QPixmap;

class WidgetWithBackground : public QWidget
{
	Q_OBJECT
  public:
     /** Konstruktor */
     // Constructor: Initializes WidgetWithBackground subsystem components, default parameters, and state.
     WidgetWithBackground(QWidget * parent = 0);

     /** Destruktor - usuwa bitmapk.  */
     // Destructor: Cleans up WidgetWithBackground resources, timers, and allocated memory.
     ~WidgetWithBackground ();

     /**
      * Operacja odmalowywania kontrolki  -
      * wkleja bitmap z tem na kontrolk -
      * generuj j wczeniej jeeli zajdzie taka potrzeba.
      */
     // Executes draw background routine.
     void  drawBackground ();

     /** Odmalowywuje kontrolk bezwarunkowo odmalowywujc to. */
     // Updates and synchronizes update with background data stream.
     void  updateWithBackground ();
    
     /** 
     * Zawraca informacje czy kontrolka zostaa zmodyfikowana
     * Ta informacja jest ustawiana gdy bitmapka ta si zmienia 
     * to znaczy zmienia si rozmiar komponentu lub istnieje 
     * potrzeba przemalowania ta. 
     */ 
     // Executes do repaint background routine.
     bool doRepaintBackground(); 
     
  protected:
    /** Wywouje paintBackground - odmalowywujc to na nowo */
    // Executes repaint background routine.
    void repaintBackground();

    /**
     * Odmalowywuje to kontrolki
     * @param painter urzdzenie na ktrym mamy malowa.
     * @param background to to kontrolki
     */
    virtual void  paintBackground (QPainter & painer) = 0;
    
    

  protected:
     /** Bufor na to. */
     QPixmap * m_pixmap;
     /**
     * Ustawia t zmienn po zmianie waciwoci
     */
     bool m_modified;
};
#endif //WIDGETWITHBACKGROUND_H

