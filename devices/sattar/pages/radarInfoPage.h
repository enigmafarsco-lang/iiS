// ============================================================================
// File: radarInfoPage.h
// Subsystem: eLynxSDR pages
// Description: Subsystem implementation and interface guidance for radarInfoPage.h.
// ============================================================================

#ifndef RADARINFOPAGE_H
#define RADARINFOPAGE_H

#include <QWidget>
#include <iostream>



namespace Ui {
class RadarInfoPage;
}

class RadarInfoPage : public QWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes RadarInfoPage subsystem components, default parameters, and state.
    explicit RadarInfoPage(QWidget *parent = nullptr);
    // Destructor: Cleans up RadarInfoPage resources, timers, and allocated memory.
    ~RadarInfoPage();

private:
    Ui::RadarInfoPage *ui;
    // Initializes init for RadarInfoPage.
    void init();
    int radarOperation = 0;


private slots:

    // Event/Signal handler for on rad  s band clicked.
    void on_rad_SBand_clicked();
    // Event/Signal handler for on rad  x band clicked.
    void on_rad_XBand_clicked();
    // Event/Signal handler for on rad  c band clicked.
    void on_rad_CBand_clicked();
    // Event/Signal handler for on rad  k band clicked.
    void on_rad_KBand_clicked();

    // Event/Signal handler for on btn ok clicked.
    void on_btnOk_clicked();

signals:

    // Executes btn clicked signal routine.
    void btnClickedSignal(int);
};

#endif // RADARINFOPAGE_H
