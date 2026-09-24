#include "pancomponent.h"

BPanComponent::BPanComponent(QWidget *parent) : QWidget(parent)
{
    init_labels();
    init_doubleSpinBoxes();
    init_buttons();
    init_progressBar();
    init_pan();
    set_components_arrangements();

    this->setStyleSheet("background-color: " + QString(BLUE_LIGHT));

    connect(btnSetPanAngle, SIGNAL(clicked()), this, SLOT(on_btnSetPanAngle_clicked()));
}

void BPanComponent::init_labels()
{
    lblPanAngle = new QLabel("Pan Angle:");
    lblPanSpeed = new QLabel("Pan Speed:");
    lblCurrentPanAngle = new QLabel("Current Pan:");
    lblCurrentPanAngleValue = new QLabel("123.45");

    const int n = 3;
    QLabel *lstLabels[n] = {lblPanAngle, lblPanSpeed, lblCurrentPanAngle};
    for(int i = 0; i< n; ++i)
    {
        lstLabels[i]->setSizePolicy(spFixed);
        lstLabels[i]->setMinimumSize(50, 15);
        //lstLabels[i]->setMaximumSize(50, 25);
        lstLabels[i]->setFont(QFont(LABEL_FONT, 11));
        lstLabels[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        //lstLabels[i]->setStyleSheet("background-color: black; color: white");
    }

    lblCurrentPanAngleValue->setSizePolicy(spExpanding);
    //lblCurrentPanAngleValue->setMinimumSize(60, 30);
    lblCurrentPanAngleValue->setMaximumSize(80, 25);
    lblCurrentPanAngleValue->setFont(QFont(NUMBER_FONT, LABEL_FONT_SIZE, 75));
    lblCurrentPanAngleValue->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    //lblCurrentPanAngleValue->setStyleSheet("background-color: white; color: blue");
    lblCurrentPanAngleValue->setContentsMargins(3, 1, 3, 1);
}

void BPanComponent::init_doubleSpinBoxes()
{
    dsbPanAngle = new QDoubleSpinBox;
    dsbPanSpeed = new QDoubleSpinBox;

    const int n = 2;
    QDoubleSpinBox *lstDoubleSpinBoxes[n] = {dsbPanAngle, dsbPanSpeed};
    for(int i = 0; i < n; ++i)
    {
        lstDoubleSpinBoxes[i]->setSizePolicy(spExpanding);
        //lstDoubleSpinBoxes[i]->setMinimumSize(60, 30);
        lstDoubleSpinBoxes[i]->setMaximumSize(80, 25);
        lstDoubleSpinBoxes[i]->setFont(QFont(LABEL_FONT, SPINBOX_FONT_SIZE));
        lstDoubleSpinBoxes[i]->setStyleSheet("background-color: white; color: blue");
        lstDoubleSpinBoxes[i]->setMinimum(0);
    }
    dsbPanAngle->setMaximum(360);
    dsbPanSpeed->setMaximum(12);
}

void BPanComponent::init_buttons()
{
    btnSetPanAngle = new QPushButton("Set Pan Angle");
    btnSetPanSpeed = new QPushButton("Set Pan Speed");

    int iconSize = 20;
    const int n = 2;
    QPushButton *lstButtons[n] = {btnSetPanAngle, btnSetPanSpeed};
    for(int i = 0; i < n; ++i)
    {
        lstButtons[i]->setSizePolicy(spExpanding);
        //lstButtons[i]->setMinimumSize(130, 30);
        lstButtons[i]->setMaximumSize(140, 25);
        lstButtons[i]->setFont(QFont(LABEL_FONT, BUTTON_FONT_SIZE));
        lstButtons[i]->setStyleSheet("background-color: silver");
        lstButtons[i]->setIcon(QIcon(":/MyResources/Images/Icons/CheckMark-128.png"));
        lstButtons[i]->setIconSize(QSize(iconSize, iconSize));
    }
}

void BPanComponent::init_progressBar()
{
    pgbPanAngle = new QProgressBar;
    pgbPanAngle->setSizePolicy(spExpanding);
    pgbPanAngle->setMinimum(0);
    pgbPanAngle->setMaximum(100);
    pgbPanAngle->setValue(70);
    pgbPanAngle->setTextVisible(false);
    pgbPanAngle->setStyleSheet("background-color:white");
}

void BPanComponent::init_pan()
{
    panGauge = new BPanGauge;
    panGauge->setSizePolicy(spExpanding);
    panGauge->setMinimumSize(240, 240);
}

void BPanComponent::set_components_arrangements()
{
    grdMainLayout = new QGridLayout(this);
    grdMainLayout->addWidget(panGauge, 0, 0, 1, 3);
    grdMainLayout->addWidget(lblPanAngle, 1, 0, 1, 1);
    grdMainLayout->addWidget(dsbPanAngle, 1, 1, 1, 1);
    grdMainLayout->addWidget(btnSetPanAngle, 1, 2, 1, 1);
    grdMainLayout->addWidget(lblPanSpeed, 2, 0, 1, 1);
    grdMainLayout->addWidget(dsbPanSpeed, 2, 1, 1, 1);
    grdMainLayout->addWidget(btnSetPanSpeed, 2, 2, 1, 1);
    grdMainLayout->addWidget(lblCurrentPanAngle, 3, 0, 1, 1);
    grdMainLayout->addWidget(lblCurrentPanAngleValue, 3, 1, 1, 1);
    grdMainLayout->addWidget(pgbPanAngle, 3, 2, 1, 1);
}
