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
    explicit RadarInfoPage(QWidget *parent = nullptr);
    ~RadarInfoPage();

private:
    Ui::RadarInfoPage *ui;
    void init();
    int radarOperation = 0;


private slots:

    void on_rad_SBand_clicked();
    void on_rad_XBand_clicked();
    void on_rad_CBand_clicked();
    void on_rad_KBand_clicked();

    void on_btnOk_clicked();

signals:

    void btnClickedSignal(int);
};

#endif // RADARINFOPAGE_H
