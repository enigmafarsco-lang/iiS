#include "tiltcomponent.h"

BTiltComponent::BTiltComponent(QWidget *parent) : QWidget(parent)
{
    init_labels();
    init_doubleSpinBoxes();
    init_buttons();
    init_progressBar();
    init_tilt();
    set_components_arrangements();

    connect(btnSetTiltAngle, SIGNAL(clicked()), this, SLOT(on_btnSetTiltAngle_clicked()));
}

void BTiltComponent::init_labels()
{
    lblTiltAngle = new QLabel("Tilt Angle:");
    lblTiltSpeed = new QLabel("Tilt Speed:");
    lblCurrentTiltAngle = new QLabel("Current Tilt:");
    lblCurrentTiltAngleValue = new QLabel("12.34");

    const int n = 3;
    QLabel *lstLabels[n] = {lblTiltAngle, lblTiltSpeed, lblCurrentTiltAngle};
    for(int i = 0; i< n; ++i)
    {
        lstLabels[i]->setSizePolicy(spFixed);
        lstLabels[i]->setMinimumSize(50, 15);
        lstLabels[i]->setFont(QFont(LABEL_FONT, 11));
        lstLabels[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lstLabels[i]->setStyleSheet("background-color: black; color: white");
    }

    lblCurrentTiltAngleValue->setSizePolicy(spExpanding);
    //lblCurrentTiltAngleValue->setMinimumSize(60, 30);
    lblCurrentTiltAngleValue->setMaximumSize(80, 25);
    lblCurrentTiltAngleValue->setFont(QFont(NUMBER_FONT, LABEL_FONT_SIZE, 75));
    lblCurrentTiltAngleValue->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lblCurrentTiltAngleValue->setStyleSheet("background-color: white; color: blue");
    lblCurrentTiltAngleValue->setContentsMargins(3, 1, 3, 1);
}

void BTiltComponent::init_doubleSpinBoxes()
{
    dsbTiltAngle = new QDoubleSpinBox;
    dsbTiltSpeed = new QDoubleSpinBox;

    const int n = 2;
    QDoubleSpinBox *lstDoubleSpinBoxes[n] = {dsbTiltAngle, dsbTiltSpeed};
    for(int i = 0; i < n; ++i)
    {
        lstDoubleSpinBoxes[i]->setSizePolicy(spExpanding);
        //lstDoubleSpinBoxes[i]->setMinimumSize(60, 30);
        lstDoubleSpinBoxes[i]->setMaximumSize(80, 25);
        lstDoubleSpinBoxes[i]->setFont(QFont(LABEL_FONT, SPINBOX_FONT_SIZE));
        lstDoubleSpinBoxes[i]->setStyleSheet("background-color: white; color: blue");
        lstDoubleSpinBoxes[i]->setMinimum(0);
    }
    dsbTiltAngle->setMinimum(-10);
    dsbTiltAngle->setMaximum(30);
    dsbTiltSpeed->setMaximum(60);
}

void BTiltComponent::init_buttons()
{
    btnSetTiltAngle = new QPushButton("Set Tilt Angle");
    btnSetTiltSpeed = new QPushButton("Set Tilt Speed");

    int iconSize = 20;
    const int n = 2;
    QPushButton *lstButtons[n] = {btnSetTiltAngle, btnSetTiltSpeed};
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

void BTiltComponent::init_progressBar()
{
    pgbTiltAngle = new QProgressBar;
    pgbTiltAngle->setSizePolicy(spExpanding);
    pgbTiltAngle->setMinimum(0);
    pgbTiltAngle->setMaximum(100);
    pgbTiltAngle->setValue(30);
    pgbTiltAngle->setTextVisible(false);
    pgbTiltAngle->setStyleSheet("background-color:white");
}

void BTiltComponent::init_tilt()
{
    tiltGauge = new BTiltGauge;
    tiltGauge->setSizePolicy(spExpanding);
    tiltGauge->setMinimumSize(240, 240);
}

void BTiltComponent::set_components_arrangements()
{
    grdMainLayout = new QGridLayout(this);
    grdMainLayout->addWidget(tiltGauge, 0, 0, 1, 3);
    grdMainLayout->addWidget(lblTiltAngle, 1, 0, 1, 1);
    grdMainLayout->addWidget(dsbTiltAngle, 1, 1, 1, 1);
    grdMainLayout->addWidget(btnSetTiltAngle, 1, 2, 1, 1);
    grdMainLayout->addWidget(lblTiltSpeed, 2, 0, 1, 1);
    grdMainLayout->addWidget(dsbTiltSpeed, 2, 1, 1, 1);
    grdMainLayout->addWidget(btnSetTiltSpeed, 2, 2, 1, 1);
    grdMainLayout->addWidget(lblCurrentTiltAngle, 3, 0, 1, 1);
    grdMainLayout->addWidget(lblCurrentTiltAngleValue, 3, 1, 1, 1);
    grdMainLayout->addWidget(pgbTiltAngle, 3, 2, 1, 1);
}
