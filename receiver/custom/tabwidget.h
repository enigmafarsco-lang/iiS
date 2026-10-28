/**
 * @file tabwidget.h
 * @brief Subsystem module for custom in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef TABWIDGET
#define TABWIDGET

#include <QTabWidget>
#include <receiver/custom/tabbar.h>


/**
 * @class TabWidget
 * @brief High-level controller class for TabWidget.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget *parent=0):QTabWidget(parent){
        /**
         * @brief Sets the tab bar parameter and updates internal state.
         * @param TabBar Input parameter specifying TabBar (new).
         */
        setTabBar(new TabBar);
        /**
         * @brief Sets the tab position parameter and updates internal state.
         * @param QTabWidget::West Input parameter specifying QTabWidget::West (QTabWidget::West).
         */
        setTabPosition(QTabWidget::West);

}

};
#endif // TABWIDGET

