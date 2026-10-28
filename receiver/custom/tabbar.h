/**
 * @file tabbar.h
 * @brief Subsystem module for custom in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef TABBAR_H
#define TABBAR_H

#include <QApplication>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>

/**
 * @class TabBar
 * @brief High-level controller class for TabBar.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class TabBar: public QTabBar{
public:
    QSize tabSizeHint(int index) const{
        QSize s = QTabBar::tabSizeHint(index);
        s.transpose();
        return s;
    }
protected:
    void paintEvent(QPaintEvent * /*event*/){
        QStylePainter painter(this);
        QStyleOptionTab opt;

        for(int i = 0;i < count();i++)
        {
            /**
             * @brief Initializes and configures the init style option for the TabBar subsystem.
             * @param opt Input parameter specifying opt (&opt).
             * @param i Input parameter specifying i (i).
             */
            initStyleOption(&opt,i);
            painter.drawControl(QStyle::CE_TabBarTabShape, opt);
            painter.save();

            QSize s = opt.rect.size();
            s.transpose();
            /**
             * @brief Executes the r procedure for TabBar.
             * @param QPoint( Input parameter specifying QPoint( (QPoint().
             * @return Output value of type QRect.
             */
            QRect r(QPoint(), s);
            r.moveCenter(opt.rect.center());
            opt.rect = r;

            QPoint c = tabRect(i).center();
            painter.translate(c);
            painter.rotate(90);
            painter.translate(-c);
            painter.drawControl(QStyle::CE_TabBarTabLabel,opt);
            painter.restore();
        }
    }
};

#endif // TABBAR_H
