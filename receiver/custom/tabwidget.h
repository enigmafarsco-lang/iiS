// ============================================================================
// File: tabwidget.h
// Subsystem: eLynxSDR custom
// Description: Header definitions and interface declarations for tabwidget.h.
// ============================================================================

#ifndef TABWIDGET
#define TABWIDGET

#include <QTabWidget>
#include <receiver/custom/tabbar.h>


class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget *parent=0):QTabWidget(parent){
        setTabBar(new TabBar);
        setTabPosition(QTabWidget::West);

}

};
#endif // TABWIDGET

