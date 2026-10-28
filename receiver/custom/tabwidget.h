// ============================================================================
// File: tabwidget.h
// Subsystem: eLynxSDR custom
// Description: Subsystem implementation and interface guidance for tabwidget.h.
// ============================================================================

#ifndef TABWIDGET
#define TABWIDGET

#include <QTabWidget>
#include <receiver/custom/tabbar.h>


class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget *parent=0):QTabWidget(parent){
        // Sets tab bar value and updates internal subsystem configuration.
        setTabBar(new TabBar);
        // Sets tab position value and updates internal subsystem configuration.
        setTabPosition(QTabWidget::West);

}

};
#endif // TABWIDGET

