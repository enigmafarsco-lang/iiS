//#include "controlunitadrv9009.h"
//#include "ui_controlunitadrv9009.h"

//#pragma region Constructor {

//ControlUnitADRV9009::ControlUnitADRV9009(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::ControlUnitADRV9009)
//{
//    ui->setupUi(this);

//    writeItemAddress.insert("th0",0x100);
//    writeItemAddress.insert("en0",0x104);
//    writeItemAddress.insert("th1",0x108);
//    writeItemAddress.insert("th2",0x10C);
//    writeItemAddress.insert("pdw",0x110);
//    writeItemAddress.insert("logouti",0x114);
//    writeItemAddress.insert("inchannel",0x118);
//    writeItemAddress.insert("thcw",0x11C);
//    writeItemAddress.insert("th1cw",0x120);
//    writeItemAddress.insert("th2cw",0x124);

//    readItemAddress.insert("cfar0",0x128);
//    readItemAddress.insert("cfar1",0x12C);

//    writeItemValues.insert("th0",0x100);
//    writeItemValues.insert("en0",0x104);
//    writeItemValues.insert("th1",0x108);
//    writeItemValues.insert("th2",0x10C);
//    writeItemValues.insert("pdw",0x110);
//    writeItemValues.insert("logouti",0x114);
//    writeItemValues.insert("inchannel",0x118);
//    writeItemValues.insert("thcw",0x11C);
//    writeItemValues.insert("th1cw",0x120);
//    writeItemValues.insert("th2cw",0x124);

//    readItemValues.insert("cfar0",0x128);
//    readItemValues.insert("cfar1",0x12C);

//    //saeid raziani
//    logouti = ui->cmbLOGOUTi;
//    //


//    QObject::connect(ui->txtTH0,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th0"]=value;});
//    QObject::connect(ui->txtTH1,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th1"]=value;});
//    QObject::connect(ui->txtTH2,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th2"]=value;});
//    QObject::connect(ui->txtInChannel,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](double value){writeItemValues["inchannel"]=value;});
//    QObject::connect(ui->txtTH0CW,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](double value){writeItemValues["thcw"]=value;});
//    QObject::connect(ui->txtTH1CW,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th1cw"]=value;});
//    QObject::connect(ui->txtTH2CW,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th2cw"]=value;});
//    QObject::connect(ui->cmbLOGOUTi,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](int value){writeItemValues["logouti"]=value;});
//    QObject::connect(ui->chkEN0,static_cast<void(QCheckBox::*)(int)>(&QCheckBox::stateChanged),[=](int value){writeItemValues["en0"]=value;});
//    QObject::connect(ui->chkPDW,static_cast<void(QCheckBox::*)(int)>(&QCheckBox::stateChanged),[=](int value){writeItemValues["pdw"]=value;});

//    QObject::connect(ui->btnRead,&QPushButton::clicked,this,&ControlUnitADRV9009::ReadAll);
//    QObject::connect(ui->btnWrite,&QPushButton::clicked,this,&ControlUnitADRV9009::WriteAll);
//    QObject::connect(ui->btnWrite_th0,&QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th0"),(uint32_t)writeItemValues.value("th0"));});
//    QObject::connect(ui->btnWrite_en0,&QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("en0"),(uint32_t)writeItemValues.value("en0"));});
//    QObject::connect(ui->btnWrite_th1,&QPushButton::clicked,
//                     [=](){
//        Write((uint32_t)writeItemAddress.value("th1"),(uint32_t)writeItemValues.value("th1"));});
//    QObject::connect(ui->btnWrite_th2,&QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th2"),(uint32_t)writeItemValues.value("th2"));});
//    QObject::connect(ui->btnWrite_pdw,&QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("pdw"),(uint32_t)writeItemValues.value("pdw"));});
//    QObject::connect(ui->btnWrite_logout,&QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("logouti"),(uint32_t)writeItemValues.value("logouti"));});
//    QObject::connect(ui->btnWrite_inChannel,&QPushButton::clicked,[=](){Write((uint64_t)writeItemAddress.value("inchannel"),(uint64_t)writeItemValues.value("inchannel"));});
//    QObject::connect(ui->btnWrite_thcw,&QPushButton::clicked,[=](){
//        Write((uint32_t)writeItemAddress.value("thcw"),(uint32_t)writeItemValues.value("thcw"));});
//    QObject::connect(ui->btnWrite_th1cw,&QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th1cw"),(uint32_t)writeItemValues.value("th1cw"));});
//    QObject::connect(ui->btnWrite_th2cw,&QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th2cw"),(uint32_t)writeItemValues.value("th2cw"));});

//    readDev = iio_context_find_device(globals::ctx, readDevice.toLocal8Bit().data());
//    writeDev = iio_context_find_device(globals::ctx, writeDevice.toLocal8Bit().data());

//    //saeid raziani
//    // first approach
//    TH1 = findChild<QSpinBox *> ("txtTH1");
//    TH2 = findChild<QSpinBox *> ("txtTH2");
//    // second approach
//    btnTh1 = ui->btnWrite_th1;
//    btnTh2 = ui->btnWrite_th2;

//    THCW1 = ui->txtTH0CW;
//    THCW2 = ui->txtTH1CW;
//    THCW3 = ui->txtTH2CW;

//    btnThCw1 = ui->btnWrite_thcw;
//    btnThCw2 = ui->btnWrite_th1cw;
//    btnThCw3 = ui->btnWrite_th2cw;
//    btnWrite = ui->btnWrite;


//    ui->cmbLOGOUTi->setCurrentIndex(28);
//    ui->cmbLOGOUTi->setCurrentIndex(29);
//    //
//}

//ControlUnitADRV9009::~ControlUnitADRV9009()
//{
//    delete ui;
//}

//#pragma endregion }

//#pragma region Operation {

///**
// * @brief ControlUnitADRV9009::ReadAll
// */
//void ControlUnitADRV9009::ReadAll()
//{
//    int ret;
//    uint32_t value;
//    uint32_t address;

//    char label[256];
//    iio_device_attr_read(readDev, attr_name.toLocal8Bit().data(), label,sizeof(label));

//    QString status=label;
//    if(status=="Disabled")
//        iio_device_attr_write(readDev, attr_name.toLocal8Bit().data(), "disabled");

//    for(auto e : readItemAddress.keys())
//    {
//        address=(uint32_t)readItemAddress.value(e);
//        ret = iio_device_reg_read(writeDev, address, &value);
//        readItemAddress[e]=ret == 0 ? value : -1;
//    }

//    ui->txtCFAR0->setValue(readItemAddress["cfar0"]);
//    ui->txtCFAR1->setValue(readItemAddress["cfar1"]);
//}

///**
// * @brief ControlUnitADRV9009::EnableWrite
// */
//void ControlUnitADRV9009::EnableWrite()
//{
//    iio_device_attr_write(writeDev, attr_name.toLocal8Bit().data(), "enabled");
//}

//void ControlUnitADRV9009::DisableWrite()
//{
//    iio_device_attr_write(writeDev, attr_name.toLocal8Bit().data(), "disabled");
//    //    iio_device_attr_write(readDev, attr_name.toLocal8Bit().data(), "disabled");
//}

///**
// * @brief ControlUnitADRV9009::WriteAll
// */
//void ControlUnitADRV9009::WriteAll()
//{
//    EnableWrite();
//    for(auto e : writeItemAddress.keys())
//        Write((uint32_t)writeItemAddress.value(e),(uint32_t)writeItemValues.value(e));
//    DisableWrite();
//}

///**
// * @brief ControlUnitADRV9009::Write
// * @param address
// * @param val
// */
//void ControlUnitADRV9009::Write(uint32_t address,uint32_t val)
//{

//    EnableWrite();
//    iio_device_reg_write(writeDev, address, val);
//    DisableWrite();
//}

//#pragma endregion }



#include "controlunitadrv9009.h"
#include "ui_controlunitadrv9009.h"

#pragma region Constructor {

ControlUnitADRV9009::ControlUnitADRV9009(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlUnitADRV9009)
{
    ui->setupUi(this);

    writeItemAddress.insert("amp",0x100);
    writeItemAddress.insert("en0",0x104);
    writeItemAddress.insert("th1",0x108);
    writeItemAddress.insert("th2",0x10C);
    writeItemAddress.insert("pdw",0x110);
    writeItemAddress.insert("logouti",0x114);
    writeItemAddress.insert("inchannel",0x118);
    writeItemAddress.insert("thcw",0x11C);
    writeItemAddress.insert("th1cw",0x120);
    writeItemAddress.insert("th2cw",0x124);


    //============== saeid raziani ===========================


    writeItemAddress.insert("dacselect",0x138);
    writeItemAddress.insert("pdiv",0x134);
    writeItemAddress.insert("chselect",0x13c);
    writeItemAddress.insert("teta_az",0x140);
    writeItemAddress.insert("Num_Side",0x144);
    writeItemAddress.insert("Num_Back",0x148);
    writeItemAddress.insert("Teta_Azo",0x154);
    writeItemAddress.insert("Num_Sideo",0x158);
    writeItemAddress.insert("Num_Backo",0x15C);
    writeItemAddress.insert("En_PATTERN",0x104);
    writeItemAddress.insert("NPS",0x128);
    writeItemAddress.insert("P_to_B",0x12C);
    writeItemAddress.insert("P_to_S",0x130);
    writeItemAddress.insert("th0",0x100);


    //=========================================


    readItemAddress.insert("cfar0",0x128);
    readItemAddress.insert("cfar1",0x12C);

    writeItemValues.insert("th0",0x100);
    writeItemValues.insert("en0",0x104);
    writeItemValues.insert("th1",0x108);
    writeItemValues.insert("th2",0x10C);
    writeItemValues.insert("pdw",0x110);
    writeItemValues.insert("logouti",0x114);
    writeItemValues.insert("inchannel",0x118);
    writeItemValues.insert("thcw",0x11C);
    writeItemValues.insert("th1cw",0x120);
    writeItemValues.insert("th2cw",0x124);

    readItemValues.insert("cfar0",0x128);
    readItemValues.insert("cfar1",0x12C);



    QObject::connect(ui->txtTH0,       static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th0"]=value;});
    QObject::connect(ui->txtTH1,       static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th1"]=value;});
    QObject::connect(ui->txtTH2,       static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th2"]=value;});
    QObject::connect(ui->txtInChannel, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](double value){writeItemValues["inchannel"]=value;});
    QObject::connect(ui->txtTH0CW,     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](double value){writeItemValues["thcw"]=value;});
    QObject::connect(ui->txtTH1CW,     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th1cw"]=value;});
    QObject::connect(ui->txtTH2CW,     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int value){writeItemValues["th2cw"]=value;});
    QObject::connect(ui->cmbLOGOUTi,   static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](int value){writeItemValues["logouti"]=value;});
    QObject::connect(ui->chkEN0,       static_cast<void(QCheckBox::*)(int)>(&QCheckBox::stateChanged),[=](int value){writeItemValues["en0"]=value;});
    QObject::connect(ui->chkPDW,       static_cast<void(QCheckBox::*)(int)>(&QCheckBox::stateChanged),[=](int value){writeItemValues["pdw"]=value;});

    QObject::connect(ui->btnRead,            &QPushButton::clicked,this,&ControlUnitADRV9009::ReadAll);
    QObject::connect(ui->btnWrite,           &QPushButton::clicked,this,&ControlUnitADRV9009::WriteAll);
    QObject::connect(ui->btnWrite_th0,       &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th0"),(uint32_t)writeItemValues.value("th0"));});
    QObject::connect(ui->btnWrite_en0,       &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("en0"),(uint32_t)writeItemValues.value("en0"));});
    QObject::connect(ui->btnWrite_th1,       &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th1"),(uint32_t)writeItemValues.value("th1"));});
    QObject::connect(ui->btnWrite_th2,       &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th2"),(uint32_t)writeItemValues.value("th2"));});
    QObject::connect(ui->btnWrite_pdw,       &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("pdw"),(uint32_t)writeItemValues.value("pdw"));});
    QObject::connect(ui->btnWrite_logout,    &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("logouti"),(uint32_t)writeItemValues.value("logouti"));});
    QObject::connect(ui->btnWrite_inChannel, &QPushButton::clicked,[=](){Write((uint64_t)writeItemAddress.value("inchannel"),(uint64_t)writeItemValues.value("inchannel"));});
    QObject::connect(ui->btnWrite_thcw,      &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("thcw"),(uint32_t)writeItemValues.value("thcw"));});
    QObject::connect(ui->btnWrite_th1cw,     &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th1cw"),(uint32_t)writeItemValues.value("th1cw"));});
    QObject::connect(ui->btnWrite_th2cw,     &QPushButton::clicked,[=](){Write((uint32_t)writeItemAddress.value("th2cw"),(uint32_t)writeItemValues.value("th2cw"));});



    //---------------------------- saeid raziani ---------------------------------------
    //==================================================================================
    //=========================== [smart noise] ========================================
    //==================================================================================

    //
    QObject::connect(this, &ControlUnitADRV9009::sendTransmitSignal, [=](int index)
    {
        Write((uint32_t)writeItemAddress.value("dacselect"),index);
//        emit sendFileToDacSignal();
    });

    //setting auto amp value
    QObject::connect(this, &ControlUnitADRV9009::setAutoAmpSignal, [=](int val)
    {
        Write((uint32_t)writeItemAddress.value("th0"),val);
    });


    //setting channel
    QObject::connect(this, &ControlUnitADRV9009::setChannelSignal, [=](double val)
    {
        Write((uint32_t)writeItemAddress.value("chselect"),val);
    });

    QObject::connect(this, &ControlUnitADRV9009::setNoiseSignal, [=](int val)
    {
        Write((uint32_t)writeItemAddress.value("chselect"),1);
    });


    QObject::connect(this, &ControlUnitADRV9009::setChannelFromTable, [=](double val)
    {
        Write((uint32_t)writeItemAddress.value("chselect"),val);
//        Write((uint32_t)writeItemAddress.value("chselect"),val-1);
//        Write((uint32_t)writeItemAddress.value("chselect"),val-2);
//        Write((uint32_t)writeItemAddress.value("chselect"),5);
//        Write((uint32_t)writeItemAddress.value("chselect"),10);
    });



    QObject::connect(this, &ControlUnitADRV9009::setInchannelSignal, [=](double val)
    {
        ui->txtInChannel->setValue(val);
        ui->btnWrite_inChannel->clicked(true);
    });


    // --------------------------------------------- scan connects ---------------------------------------------
    QObject::connect(this,  &ControlUnitADRV9009::setPdivSignal,     [=](int val){
        Write((uint32_t)writeItemAddress.value("pdiv"),         val);});

    QObject::connect(this,  &ControlUnitADRV9009::setteTa_azSignal,  [=](int val){
        Write((uint32_t)writeItemAddress.value("teta_az"),      val);});

    QObject::connect(this,  &ControlUnitADRV9009::setNum_SideSignal, [=](int val){
        Write((uint32_t)writeItemAddress.value("Num_Side"),     val);});

    QObject::connect(this,  &ControlUnitADRV9009::setNum_BackSignal, [=](int val){
        Write((uint32_t)writeItemAddress.value("Num_Back"),     val);});

    QObject::connect(this,  &ControlUnitADRV9009::setNPSSignal,      [=](int val){
        Write((uint32_t)writeItemAddress.value("NPS"),          val);});

    QObject::connect(this,  &ControlUnitADRV9009::setP_to_BSignal,   [=](int val){
        Write((uint32_t)writeItemAddress.value("P_to_B"),       val);});

    QObject::connect(this,  &ControlUnitADRV9009::setP_to_SSignal,   [=](int val){
        Write((uint32_t)writeItemAddress.value("P_to_S"),       val);});

    QObject::connect(this,  &ControlUnitADRV9009::enPatternSignal,   [=](int val){
        Write((uint32_t)writeItemAddress.value("En_PATTERN"),   val);});

    //------------------------------------------------------------------------------------------------------------------------------------



//    //send maximum frq to card
//    QObject::connect(this, &ControlUnitADRV9009::setChannelFromTable, [=](double val)
//    {
//        Write((uint32_t)writeItemAddress.value("chselect"),val);
//    });

    //=========================================================================================



    // Resolve the register bridge only after the IIO context exists.
    // Besides the historical mwipcore0:mmwr0/mmrd0 names, tolerate renamed
    // HDL-Coder bridges by inspecting IIO device id/name for mmwr/mmrd.
    resolveRegisterDevices();

    if (!readDev)
        qWarning() << "ADRV9009 Control Unit: IIO register read bridge not found. Expected"
                   << readDevice << "or an IIO device containing 'mmrd'.";

    if (!writeDev)
        qWarning() << "ADRV9009 Control Unit: IIO register write bridge not found. Expected"
                   << writeDevice << "or an IIO device containing 'mmwr'."
                   << "led_count_ip AXI base is 0x43C30000.";

    writeItemValues["th0"]=ui->txtTH0->value();
    writeItemValues["th1"]=ui->txtTH1->value();
    writeItemValues["th2"]=ui->txtTH2->value();
    writeItemValues["inchannel"]=(uint64_t)ui->txtInChannel->value();
    writeItemValues["thcw"]=ui->txtTH0CW->value();
    writeItemValues["th1cw"]=ui->txtTH1CW->value();
    writeItemValues["th2cw"]=ui->txtTH2CW->value();
    writeItemValues["logouti"]=ui->cmbLOGOUTi->currentIndex();
    writeItemValues["en0"]=ui->chkEN0->isChecked();
    writeItemValues["pdw"]=ui->chkPDW->isChecked();


    //    //saeid raziani
    // first approach
    TH1 = findChild<QSpinBox *> ("txtTH1");
    TH2 = findChild<QSpinBox *> ("txtTH2");
    // second approach
    btnTh1 = ui->btnWrite_th1;
    btnTh2 = ui->btnWrite_th2;

    THCW1 = ui->txtTH0CW;
    THCW2 = ui->txtTH1CW;
    THCW3 = ui->txtTH2CW;

//    btnWrite_th1 = ui->btnWrite_th1;
//    btnWrite_th2 = ui->btnWrite_th2;

    btnThCw1 = ui->btnWrite_thcw;
    btnThCw2 = ui->btnWrite_th1cw;
    btnThCw3 = ui->btnWrite_th2cw;
    btnWrite = ui->btnWrite;


    ui->cmbLOGOUTi->setCurrentIndex(28);
    ui->cmbLOGOUTi->setCurrentIndex(29);
    //

}

ControlUnitADRV9009::~ControlUnitADRV9009()
{
    delete ui;
}

#pragma endregion }

#pragma region Operation {

/**
 * @brief ControlUnitADRV9009::ReadAll
 */
void ControlUnitADRV9009::ReadAll()
{
    if (!readDev || !writeDev)
    {
        qWarning() << "ADRV9009 Control Unit: ReadAll skipped because required IIO device is missing"
                   << "readDev=" << (readDev != nullptr)
                   << "writeDev=" << (writeDev != nullptr);
        return;
    }

    int ret;
    uint32_t value;
    uint32_t address;

    char label[256] = {};
    const QByteArray attrName = attr_name.toLocal8Bit();
    iio_device_attr_read(readDev, attrName.constData(), label,sizeof(label));

    QString status=label;
    if(status=="Disabled")
        iio_device_attr_write(readDev, attrName.constData(), "disabled");

    for(auto e : readItemAddress.keys())
    {
        address=(uint32_t)readItemAddress.value(e);
        ret = iio_device_reg_read(writeDev, address, &value);
        readItemAddress[e]=ret == 0 ? value : -1;
    }

    ui->txtCFAR0->setValue(readItemAddress["cfar0"]);
    ui->txtCFAR1->setValue(readItemAddress["cfar1"]);
}

/**
 * @brief ControlUnitADRV9009::EnableWrite
 */
void ControlUnitADRV9009::EnableWrite()
{
    resolveRegisterDevices();
    if (!writeDev)
    {
        qWarning() << "ADRV9009 Control Unit: EnableWrite skipped; IIO write bridge not found.";
        return;
    }

    const QByteArray attrName = attr_name.toLocal8Bit();
    if (!iio_device_find_attr(writeDev, attrName.constData()))
        return;

    const ssize_t ret = iio_device_attr_write(writeDev, attrName.constData(), "enabled");
    if (ret < 0)
        qWarning() << "ADRV9009 Control Unit: failed to enable register access, error" << ret;
}

void ControlUnitADRV9009::DisableWrite()
{
    resolveRegisterDevices();
    if (!writeDev)
        return;

    const QByteArray attrName = attr_name.toLocal8Bit();
    if (!iio_device_find_attr(writeDev, attrName.constData()))
        return;

    const ssize_t ret = iio_device_attr_write(writeDev, attrName.constData(), "disabled");
    if (ret < 0)
        qWarning() << "ADRV9009 Control Unit: failed to disable register access, error" << ret;
}

QString ControlUnitADRV9009::deviceDisplayName(struct iio_device *dev) const
{
    if (!dev)
        return QStringLiteral("<none>");

    const char *name = iio_device_get_name(dev);
    const char *id = iio_device_get_id(dev);
    if (name && *name)
        return QString::fromLocal8Bit(name);
    if (id && *id)
        return QString::fromLocal8Bit(id);
    return QStringLiteral("<unnamed IIO device>");
}

void ControlUnitADRV9009::resolveRegisterDevices()
{
    readDev = nullptr;
    writeDev = nullptr;

    if (!globals::ctx)
        return;

    const QByteArray exactRead = readDevice.toLocal8Bit();
    const QByteArray exactWrite = writeDevice.toLocal8Bit();
    readDev = iio_context_find_device(globals::ctx, exactRead.constData());
    writeDev = iio_context_find_device(globals::ctx, exactWrite.constData());

    const unsigned int count = iio_context_get_devices_count(globals::ctx);
    for (unsigned int i = 0; i < count && (!readDev || !writeDev); ++i)
    {
        struct iio_device *dev = iio_context_get_device(globals::ctx, i);
        if (!dev)
            continue;

        const QString name = QString::fromLocal8Bit(iio_device_get_name(dev) ? iio_device_get_name(dev) : "");
        const QString id   = QString::fromLocal8Bit(iio_device_get_id(dev)   ? iio_device_get_id(dev)   : "");
        const QString key = (name + QLatin1Char(' ') + id).toLower();

        const QByteArray regAccessName = attr_name.toLocal8Bit();
        const bool hasRegAccess = iio_device_find_attr(dev, regAccessName.constData()) != nullptr;
        const bool looksLikeLedCount = key.contains(QStringLiteral("led_count"));
        const bool looksLikeMwipcore = key.contains(QStringLiteral("mwipcore"));

        if (!writeDev && (key.contains(QStringLiteral("mmwr")) ||
                          key.contains(QStringLiteral("memorywrite")) ||
                          (looksLikeMwipcore && hasRegAccess) ||
                          (looksLikeLedCount && hasRegAccess)))
            writeDev = dev;

        if (!readDev && (key.contains(QStringLiteral("mmrd")) ||
                         key.contains(QStringLiteral("memoryread")) ||
                         (looksLikeLedCount && hasRegAccess)))
            readDev = dev;
    }

    // Some HDL-Coder/IIO bridges expose one bidirectional register device.
    // Only reuse it for readback when no dedicated read bridge is present.
    if (!readDev && writeDev)
        readDev = writeDev;
}

QString ControlUnitADRV9009::registerBackendInfo()
{
    resolveRegisterDevices();
    return QStringLiteral("AXI led_count_ip base 0x%1 | write bridge: %2 | read bridge: %3")
            .arg(QString::number(LED_COUNT_AXI_BASE, 16).toUpper())
            .arg(deviceDisplayName(writeDev))
            .arg(deviceDisplayName(readDev));
}

bool ControlUnitADRV9009::tryReadRegister(quint32 offset, quint32 *value)
{
    if (!value)
        return false;

    resolveRegisterDevices();

    struct iio_device *candidates[2] = { readDev, writeDev };
    for (int i = 0; i < 2; ++i)
    {
        struct iio_device *dev = candidates[i];
        if (!dev || (i == 1 && dev == candidates[0]))
            continue;

        uint32_t readValue = 0;
        const int ret = iio_device_reg_read(dev, offset, &readValue);
        if (ret == 0)
        {
            *value = readValue;
            return true;
        }
    }
    return false;
}

bool ControlUnitADRV9009::writeRegisterVerified(quint32 offset, quint32 value,
                                                const QString &label,
                                                QString *details)
{
    resolveRegisterDevices();

    const quint32 physicalAddress = LED_COUNT_AXI_BASE + offset;
    const QString addressText = QStringLiteral("0x%1")
            .arg(QString::number(physicalAddress, 16).toUpper(), 8, QLatin1Char('0'));
    const QString rawText = QStringLiteral("0x%1")
            .arg(QString::number(value, 16).toUpper(), 8, QLatin1Char('0'));
    const QString offsetText = QStringLiteral("0x%1")
            .arg(QString::number(offset, 16).toUpper(), 3, QLatin1Char('0'));

    if (!writeDev)
    {
        if (details)
        {
            *details = QStringLiteral("%1 FAILED | AXI %2 [offset %3] <= %4 (%5). "
                                      "No IIO AXI register bridge was found. "
                                      "Expected mwipcore0:mmwr0 (or compatible mmwr) for led_count_ip. "
                                      "The FPGA address map alone is not network-accessible until the Linux/IIO bridge is exposed.")
                    .arg(label)
                    .arg(addressText)
                    .arg(offsetText)
                    .arg(QString::number(value))
                    .arg(rawText);
        }
        qWarning() << "ADRV9009 Control Unit:" << (details ? *details : label);
        return false;
    }

    // The generated MathWorks register bridge historically requires reg_access
    // to be enabled. Other compatible bridges may not expose this attribute,
    // in which case iio_device_reg_write is used directly.
    const QByteArray attrName = attr_name.toLocal8Bit();
    const bool hasRegAccessAttr = iio_device_find_attr(writeDev, attrName.constData()) != nullptr;
    if (hasRegAccessAttr)
    {
        const ssize_t enableRet = iio_device_attr_write(writeDev, attrName.constData(), "enabled");
        if (enableRet < 0)
            qWarning() << "ADRV9009 Control Unit: reg_access enable returned" << enableRet;
    }

    const int ret = iio_device_reg_write(writeDev, offset, value);

    if (hasRegAccessAttr)
        iio_device_attr_write(writeDev, attrName.constData(), "disabled");

    if (ret < 0)
    {
        if (details)
        {
            *details = QStringLiteral("%1 FAILED | AXI %2 [offset %3] <= %4 (%5), IIO error %6, bridge %7")
                    .arg(label)
                    .arg(addressText)
                    .arg(offsetText)
                    .arg(QString::number(value))
                    .arg(rawText)
                    .arg(ret)
                    .arg(deviceDisplayName(writeDev));
        }
        qWarning() << "ADRV9009 Control Unit:" << (details ? *details : label);
        return false;
    }

    // In led_count_ip_addr_decoder.v these control registers are write-only:
    // the AXI read mux exposes ip_timestamp/CFAR/Teta/Num outputs, but not
    // TH0/PDW/thcw/DACseles. Therefore a zero bus read here would NOT mean
    // that the write failed. Report the accepted write instead of presenting
    // a misleading read-back value.
    const bool ledCountWriteOnly =
            offset == REG_AMPLY ||
            offset == REG_VGPO_ENABLE ||
            offset == REG_VGPO_OFFSET ||
            offset == REG_VGPO_STEP ||
            offset == REG_DRFM_SELECT;

    if (details)
    {
        if (ledCountWriteOnly)
        {
            *details = QStringLiteral("%1 OK | AXI %2 [offset %3] <= %4 (%5) | IIO write accepted | "
                                      "led_count_ip register is write-only (no hardware read-back path) | bridge %6")
                    .arg(label)
                    .arg(addressText)
                    .arg(offsetText)
                    .arg(QString::number(value))
                    .arg(rawText)
                    .arg(deviceDisplayName(writeDev));
        }
        else
        {
            quint32 readBack = 0;
            const bool readOk = tryReadRegister(offset, &readBack);
            if (readOk)
            {
                const QString rbHex = QStringLiteral("0x%1")
                        .arg(QString::number(readBack, 16).toUpper(), 8, QLatin1Char('0'));
                *details = QStringLiteral("%1 OK | AXI %2 <= %3 (%4) | read-back %5 (%6) | bridge %7")
                        .arg(label)
                        .arg(addressText)
                        .arg(QString::number(value))
                        .arg(rawText)
                        .arg(QString::number(readBack))
                        .arg(rbHex)
                        .arg(deviceDisplayName(writeDev));
            }
            else
            {
                *details = QStringLiteral("%1 WRITE OK | AXI %2 <= %3 (%4) | read-back unavailable | bridge %5")
                        .arg(label)
                        .arg(addressText)
                        .arg(QString::number(value))
                        .arg(rawText)
                        .arg(deviceDisplayName(writeDev));
            }
        }
    }

    return true;
}

bool ControlUnitADRV9009::writeRegisterLikeThreshold(quint32 offset, quint32 value,
                                                       const QString &label,
                                                       QString *details)
{
    // IMPORTANT: this is deliberately the same transaction used by the
    // original TH1/TH2 Write buttons: resolve mwipcore -> enable reg_access ->
    // iio_device_reg_write(offset,value) -> disable reg_access.
    resolveRegisterDevices();

    const quint32 physicalAddress = LED_COUNT_AXI_BASE + offset;
    const QString addrText = QStringLiteral("0x%1")
            .arg(QString::number(physicalAddress, 16).toUpper(), 8, QLatin1Char('0'));
    const QString offText = QStringLiteral("0x%1")
            .arg(QString::number(offset, 16).toUpper(), 3, QLatin1Char('0'));
    const QString valueHex = QStringLiteral("0x%1")
            .arg(QString::number(value, 16).toUpper(), 8, QLatin1Char('0'));

    if (!writeDev)
    {
        if (details)
            *details = QStringLiteral("%1 FAILED | AXI %2 [offset %3] <= %4 (%5) | no mwipcore/mmwr IIO bridge")
                    .arg(label, addrText, offText, QString::number(value), valueHex);
        qWarning() << "ADRV9009 Control Unit:" << (details ? *details : label);
        return false;
    }

    const QByteArray attrName = attr_name.toLocal8Bit();
    const bool hasRegAccess = iio_device_find_attr(writeDev, attrName.constData()) != nullptr;
    if (hasRegAccess)
    {
        const ssize_t er = iio_device_attr_write(writeDev, attrName.constData(), "enabled");
        if (er < 0)
            qWarning() << "ADRV9009 Control Unit: reg_access enable failed" << er;
    }

    const int ret = iio_device_reg_write(writeDev, offset, value);

    if (hasRegAccess)
        iio_device_attr_write(writeDev, attrName.constData(), "disabled");

    if (details)
    {
        if (ret == 0)
            *details = QStringLiteral("%1 OK | AXI %2 [offset %3] <= %4 (%5) | bridge %6")
                    .arg(label, addrText, offText, QString::number(value), valueHex,
                         deviceDisplayName(writeDev));
        else
            *details = QStringLiteral("%1 FAILED | AXI %2 [offset %3] <= %4 (%5) | IIO error %6 | bridge %7")
                    .arg(label, addrText, offText, QString::number(value), valueHex,
                         QString::number(ret), deviceDisplayName(writeDev));
    }

    if (ret < 0)
        qWarning() << "ADRV9009 Control Unit:" << (details ? *details : label);
    else
        qInfo() << "ADRV9009 Control Unit:" << (details ? *details : label);

    return ret == 0;
}

bool ControlUnitADRV9009::setDrfmEnabled(bool enabled, QString *details)
{
    // tcl07020.tcl:
    // led_count_ip_0/DACseleso -> MUXDACS_ip_0/DacSel.
    // MUXDACS_ip.v final mux: DacSel=1 => DRFM/processed path,
    // DacSel=0 => DMA path used by the noise waveform.
    const quint32 value = enabled ? 1u : 0u;
    writeItemValues["dacselect"] = value;
    return writeRegisterLikeThreshold(REG_DRFM_SELECT, value,
                                      enabled ? QStringLiteral("DRFM ON / DACSEL=1")
                                              : QStringLiteral("NOISE / DACSEL=0"),
                                      details);
}

bool ControlUnitADRV9009::setAmplifyValue(quint16 value, bool enabled, QString *details)
{
    // TH0 is wired directly to MUXDACS amplify.  There is no separate
    // amplify-enable register in the supplied RTL.  OFF therefore restores
    // unity gain (1); ON writes the exact requested TH0 value.
    const quint32 appliedValue = enabled ? static_cast<quint32>(value) : 1u;
    writeItemValues["th0"] = appliedValue;

    // Keep the original ControlUnit UI in sync.  TH1/TH2 use the same
    // valueChanged -> writeItemValues -> Write register path.
    if (ui && ui->txtTH0)
        ui->txtTH0->setValue(static_cast<int>(appliedValue));

    QString local;
    const bool ok = writeRegisterLikeThreshold(REG_AMPLY, appliedValue,
                                                enabled ? QStringLiteral("AMPLY ON / TH0")
                                                        : QStringLiteral("AMPLY OFF / TH0=1"),
                                                &local);
    if (details)
    {
        *details = local;
        if (!enabled)
            *details += QStringLiteral(" | requested value %1 remains in the DRFM tab").arg(value);
    }
    return ok;
}

bool ControlUnitADRV9009::setVgpoEnabled(bool enabled, QString *details)
{
    // PDW is the real Doppler/VGPO enable line.
    const quint32 value = enabled ? 1u : 0u;
    writeItemValues["pdw"] = value;
    if (ui && ui->chkPDW)
        ui->chkPDW->setChecked(enabled);

    return writeRegisterLikeThreshold(REG_VGPO_ENABLE, value,
                                      enabled ? QStringLiteral("VGPO/DOPPLER ON / PDW=1")
                                              : QStringLiteral("VGPO/DOPPLER OFF / PDW=0"),
                                      details);
}

bool ControlUnitADRV9009::setVgpoParameters(quint32 phaseOffset, qint32 phaseStep,
                                             QString *details)
{
    // tcl07020.tcl wiring:
    //   INchann -> doppler_phase_offset  (+0x118)
    //   thcw    -> doppler_phase_step    (+0x11C)
    // Write offset first, then phase step, while enable is controlled
    // independently by PDW.
    const quint32 rawStep = static_cast<quint32>(phaseStep);
    writeItemValues["inchannel"] = phaseOffset;
    writeItemValues["thcw"] = rawStep;

    if (ui && ui->txtInChannel)
        ui->txtInChannel->setValue(static_cast<double>(phaseOffset));
    if (ui && ui->txtTH0CW &&
            static_cast<double>(phaseStep) >= ui->txtTH0CW->minimum() &&
            static_cast<double>(phaseStep) <= ui->txtTH0CW->maximum())
    {
        // Keep the legacy widget synchronized only when the value is inside
        // its original Designer range.  Do not alter that range because the
        // existing application may rely on it elsewhere.
        ui->txtTH0CW->setValue(static_cast<double>(phaseStep));
    }

    QString offsetDetails;
    const bool offsetOk = writeRegisterLikeThreshold(REG_VGPO_OFFSET, phaseOffset,
                                                      QStringLiteral("VGPO PHASE OFFSET / INchann"),
                                                      &offsetDetails);
    QString stepDetails;
    const bool stepOk = writeRegisterLikeThreshold(REG_VGPO_STEP, rawStep,
                                                    QStringLiteral("VGPO PHASE STEP / thcw"),
                                                    &stepDetails);

    if (details)
        *details = QStringLiteral("%1\n%2").arg(offsetDetails, stepDetails);
    return offsetOk && stepOk;
}

bool ControlUnitADRV9009::setVgpoValue(quint32 phaseOffset, qint32 phaseStep,
                                       bool enabled, QString *details)
{
    // Program parameters before enabling the phase accumulator, which avoids
    // one transient sample using stale values.
    QString paramDetails;
    const bool paramsOk = setVgpoParameters(phaseOffset, phaseStep, &paramDetails);

    QString enableDetails;
    const bool enableOk = setVgpoEnabled(enabled, &enableDetails);

    if (details)
        *details = QStringLiteral("VGPO offset=%1, phaseStep=%2\n%3\n%4")
                .arg(phaseOffset)
                .arg(phaseStep)
                .arg(paramDetails, enableDetails);
    return paramsOk && enableOk;
}

/**
 * @brief ControlUnitADRV9009::WriteAll
 */
void ControlUnitADRV9009::WriteAll()
{
    if (!writeDev)
    {
        qWarning() << "ADRV9009 Control Unit: WriteAll skipped; IIO write device not found:" << writeDevice;
        return;
    }

    EnableWrite();
    for(auto e : writeItemAddress.keys())
        Write((uint32_t)writeItemAddress.value(e),(uint32_t)writeItemValues.value(e));
    DisableWrite();
}

/**
 * @brief ControlUnitADRV9009::Write
 * @param address
 * @param val
 */
void ControlUnitADRV9009::Write(uint32_t address,uint32_t val)
{
    // Original TH1/TH2 and every legacy register button arrive here.
    // Route them through the same transaction used by the new tab so there
    // is only one hardware-write implementation in the application.
    QString details;
    writeRegisterLikeThreshold(address, val, QStringLiteral("LEGACY REGISTER WRITE"), &details);
}

#pragma endregion }
