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
     * a context is created exclusively after the user presses Connect.
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
        ui->radOrx400->setEnabled(true);
        if (!ui->radOrx200->isChecked() && !ui->radOrx400->isChecked())
            ui->radOrx200->setChecked(true);
    }
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
 * @brief connectDialog::btn_connect_clicked
 */
void connectDialog::btn_connect_clicked()
{
    /* Accept both "192.168.1.10" and "ip:192.168.1.10" from the user. */
    QString uri = ui->txt_uri->text().trimmed();
    if (uri.isEmpty())
    {
        ui->lblStatus->setText("Please enter the board IP address.");
        return;
    }

    if (!uri.startsWith(QStringLiteral("ip:"), Qt::CaseInsensitive))
        uri.prepend(QStringLiteral("ip:"));

    ui->txt_uri->setText(uri);

    /* Always create the context from the value currently shown in the dialog. */
    struct iio_context *ctx = iio_create_context_from_uri(uri.toLocal8Bit().constData());
    if (!ctx)
    {
        char errbuf[256] = {0};
        iio_strerror(errno, errbuf, sizeof(errbuf));
        ui->lblStatus->setText(
                    QString("Connection failed: %1").arg(QString::fromLocal8Bit(errbuf)));
        qCritical() << "IIO connection failed for" << uri << ":" << errbuf;
        return;
    }

    // This application is specifically built for the ADRV9009 receiver.
    // Keep the dialog open when the entered IP points to another IIO target.
    if (!iio_context_find_device(ctx, "adrv9009-phy"))
    {
        ui->lblStatus->setText(
                    "IIO connection succeeded, but adrv9009-phy was not found on this target.");
        qCritical() << "Connected IIO target does not expose adrv9009-phy:" << uri;
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

    qInfo() << "User confirmed IIO URI:" << uri;
    accept();
}

#pragma endregion }
