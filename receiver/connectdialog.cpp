#include "connectdialog.h"
#include "ui_connectdialog.h"


#pragma region Constructor {

connectDialog::connectDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::connectDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->btn_cancel,  &QPushButton::clicked,
                     this, &connectDialog::btn_cancel_clicked, Qt::UniqueConnection);
    QObject::connect(ui->btn_refresh, &QPushButton::clicked,
                     this, &connectDialog::btn_refresh_clicked, Qt::UniqueConnection);
    QObject::connect(ui->btn_connect, &QPushButton::clicked,
                     this, &connectDialog::btn_connect_clicked, Qt::UniqueConnection);
    QObject::connect(ui->radTx100, &QRadioButton::toggled,
                     this, &connectDialog::txBwToggled, Qt::UniqueConnection);
    QObject::connect(ui->radTx200, &QRadioButton::toggled,
                     this, &connectDialog::txBwToggled, Qt::UniqueConnection);
    QObject::connect(ui->radTx400, &QRadioButton::toggled,
                     this, &connectDialog::txBwToggled, Qt::UniqueConnection);

    syncOrxChoices();
}

connectDialog::~connectDialog()
{
    delete ui;
}

bool connectDialog::Initialize()
{
    /* Do not create an IIO context while the dialog is being initialized:
     * a context is created only after the user presses Refresh or Connect.
     * Populate the manual IP field only.
     */
    setting.ReadSettingFile();

    QString savedIp = setting.getIp().trimmed();
    if (savedIp.isEmpty())
        savedIp = QStringLiteral("192.168.1.10");

    if (!savedIp.startsWith(QStringLiteral("ip:"), Qt::CaseInsensitive))
        savedIp.prepend(QStringLiteral("ip:"));

    ui->txt_uri->setText(savedIp);
    ui->btn_connect->setFocus();

    syncOrxChoices();

    /* ReceiverMain will exec() the dialog and wait for an explicit
     * Connect action from the user. */
    return false;
}

#pragma endregion }

#pragma region Profile Selection {

int connectDialog::selectedTxBw() const
{
    if (ui->radTx400->isChecked())
        return 400;
    if (ui->radTx200->isChecked())
        return 200;
    return 100;
}

int connectDialog::selectedOrxBw() const
{
    if (ui->radOrx400->isChecked())
        return 400;
    if (ui->radOrx200->isChecked())
        return 200;
    return 100;
}

/**
 * @brief connectDialog::txBwToggled
 * @param checked
 */
void connectDialog::txBwToggled(bool checked)
{
    if (!checked)
        return;
    syncOrxChoices();
}

/**
 * Keep the ORx choices consistent with the TX choice:
 *   TX 100 -> ORx 100 only
 *   TX 200 -> ORx 100 / 200
 *   TX 400 -> ORx 200 / 400
 * If the currently checked ORx is not allowed any more, fall back to the
 * first allowed choice (ORx 100 for TX 100/200, ORx 200 for TX 400).
 */
void connectDialog::syncOrxChoices()
{
    // SN001 boards have no ORx 400: keep the option disabled (100/200
    // only) instead of showing any message.
    const bool orx400Ok = !globals::serialIsSn001(boardSerial);

    const int tx = selectedTxBw();

    if (tx == 100) {
        ui->radOrx100->setEnabled(true);
        ui->radOrx200->setEnabled(false);
        ui->radOrx400->setEnabled(false);
        if (!ui->radOrx100->isChecked())
            ui->radOrx100->setChecked(true);
    } else if (tx == 200) {
        ui->radOrx100->setEnabled(true);
        ui->radOrx200->setEnabled(true);
        ui->radOrx400->setEnabled(false);
        if (!ui->radOrx100->isChecked() && !ui->radOrx200->isChecked())
            ui->radOrx100->setChecked(true);
    } else {
        ui->radOrx100->setEnabled(false);
        ui->radOrx200->setEnabled(true);
        ui->radOrx400->setEnabled(orx400Ok);
        const bool ok400 = orx400Ok && ui->radOrx400->isChecked();
        if (!ui->radOrx200->isChecked() && !ok400)
            ui->radOrx200->setChecked(true);
    }
}

/**
 * Read the board serial number into the form and re-gate the ORx 400
 * option (SN001 boards get ORx 100/200 only).
 */
void connectDialog::updateBoardSerial()
{
    boardSerial = globals::boardSerialNumber();
    ui->txt_board_serial->setText(boardSerial.isEmpty()
                                  ? QStringLiteral("(unknown - press Refresh)")
                                  : boardSerial);
    if (!boardSerial.isEmpty())
        qInfo() << "Form: board serial number" << boardSerial;
    syncOrxChoices();
}

#pragma endregion }

#pragma region Widgets Events  {

/**
 * @brief connectDialog::btn_cancel_clicked
 */
void connectDialog::btn_cancel_clicked()
{
    this->close();
}

/**
 * @brief connectDialog::btn_refresh_clicked
 * Refresh the IIO Context Information panels from the entered IP address.
 */
void connectDialog::btn_refresh_clicked()
{
    ReloadConnectDialog();
}

/**
 * @brief connectDialog::btn_connect_clicked
 */
void connectDialog::btn_connect_clicked()
{
    /* Accept both "192.168.1.10" and "ip:192.168.1.10" from the user. */
    QString uri = ui->txt_uri->text().trimmed();
    if (uri.isEmpty())
    {
        ui->txt_context_description->setText("Please enter the board IP address.");
        return;
    }

    if (!uri.startsWith(QStringLiteral("ip:"), Qt::CaseInsensitive))
        uri.prepend(QStringLiteral("ip:"));

    ui->txt_uri->setText(uri);

    /* Always create the context from the value currently shown in the dialog.
     * Do not silently reuse an address created during dialog initialization. */
    struct iio_context *ctx = GetContext();
    if (!ctx)
    {
        char errbuf[256] = {0};
        iio_strerror(errno, errbuf, sizeof(errbuf));
        ui->txt_context_description->setText(
                    QString("Connection failed: %1").arg(QString::fromLocal8Bit(errbuf)));
        qCritical() << "IIO connection failed for" << uri << ":" << errbuf;
        return;
    }

    // This application is specifically built for the ADRV9009 receiver.
    // Keep the dialog open when the entered IP points to another IIO target.
    if (!iio_context_find_device(ctx, "adrv9009-phy"))
    {
        ui->txt_context_description->setText(
                    "IIO connection succeeded, but adrv9009-phy was not found on this target.");
        qCritical() << "Connected IIO target does not expose adrv9009-phy:" << uri;
        if (ctx != globals::ctx)
            iio_context_destroy(ctx);
        return;
    }

    if (globals::ctx && globals::ctx != ctx)
        iio_context_destroy(globals::ctx);
    globals::ctx = ctx;

    setting.setMode("manual");
    QString ip = uri;
    if (ip.startsWith(QStringLiteral("ip:"), Qt::CaseInsensitive))
        ip.remove(0, 3);
    setting.setIp(ip);
    setting.SaveToFile();

    // Check the board serial number and write it in the form before the
    // dialog closes - the startup profile assertion uses the (possibly
    // fallback-adjusted) TX/ORx selection.
    updateBoardSerial();

    qInfo() << "User confirmed IIO URI:" << uri;
    accept();
}

#pragma endregion }

#pragma region Operations {

/**
 * @brief connectDialog::GetContext
 * @return the IIO context for the URI in the form (manual entry only)
 */
iio_context *connectDialog::GetContext()
{
    QByteArray uri = ui->txt_uri->text().trimmed().toLocal8Bit();
    char *hostname = uri.data();

    if (!g_str_has_prefix(hostname, "ip:")) {
        QByteArray withPrefix = QByteArray("ip:") + uri;
        uri = withPrefix;
        hostname = uri.data();
    }

    iio_context *ctx = globals::ctx;

    if (ctx && !g_strcmp0(hostname, iio_context_get_attr_value(ctx, "uri")))
        return ctx;

    return iio_create_context_from_uri(hostname);
}

/**
 * @brief refresh connect dialog attributes
 * @return true if context found
 */
bool connectDialog::ReloadConnectDialog()
{
    QString text;
    size_t i;
    iio_context *ctx;
    QString desc;

    ctx = GetContext();

    if (!ctx) {
        char errbuf[256] = {0};
        const int errorNumber = errno;
        iio_strerror(errorNumber, errbuf, sizeof(errbuf));
        desc = QString::fromLocal8Bit(errbuf);
        qCritical() << "IIO context creation failed:" << desc
                    << "errno:" << errorNumber;
    } else {
        desc = iio_context_get_description(ctx);
        qInfo() << "IIO context ready:" << desc
                << "devices:" << iio_context_get_devices_count(ctx);
    }

    ui->txt_context_description->setText(desc);

    text="";
    if (ctx) {
        for (i = 0; i < iio_context_get_devices_count(ctx); i++) {
            iio_device *dev = iio_context_get_device(ctx, i);
            const char *name = iio_device_get_name(dev);
            if (!name)
                name = iio_device_get_id(dev);
            text += tr("%1\n").arg(QString::fromLocal8Bit(name));
        }
    } else {
        text="No context, No iio devices found\n";
    }

    ui->txt_iio_devices->setText(text);

    text="";
    if (ctx) {
        for (i = 0; i < iio_context_get_attrs_count(ctx); i++) {
            const char *key, *value;
            ssize_t ret;
            ret = iio_context_get_attr(ctx, i, &key, &value);
            if (!ret) {
                text+=tr("%1 = %2\n").arg(key).arg(value);
            }
        }
    } else {
        text="No context attributes\n";
    }
    ui->txt_context_attributes->setText(text);

    // TODO: FRU files

    if(ctx)
    {
        ui->btn_connect->setFocus();
        if(ctx!=globals::ctx)
        {
            if (globals::ctx)
                iio_context_destroy(globals::ctx);
            globals::ctx=ctx;
        }
    }

    // Check the board serial number and write it in the form; it gates
    // the ORx 400 option (SN001 boards have ORx 100/200 only).
    updateBoardSerial();

    return ctx;
}

#pragma endregion }
