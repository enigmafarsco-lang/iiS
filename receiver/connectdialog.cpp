#include "connectdialog.h"
#include "ui_connectdialog.h"


#pragma region Constructor {

connectDialog::connectDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::connectDialog)
{
    ui->setupUi(this);
}

connectDialog::~connectDialog()
{
    delete ui;
}

bool connectDialog::Initialize()
{
    /*
     * Do not create an IIO context while the dialog is being initialized.
     * The previous implementation called ReloadConnectDialog() here, which
     * immediately connected to the hard-coded/saved address and caused
     * ReceiverMain to skip the dialog completely.
     *
     * Populate the manual URI field only.  A context is created exclusively
     * after the user presses Connect.
     */
    setting.ReadSettingFile();

    QString savedIp = setting.getIp().trimmed();
    if (savedIp.isEmpty())
        savedIp = QStringLiteral("192.168.1.10");

    if (!savedIp.startsWith(QStringLiteral("ip:"), Qt::CaseInsensitive))
        savedIp.prepend(QStringLiteral("ip:"));

    /* Avoid the auto-connected rad_manual_toggled() slot from probing the
     * network while we are only initializing the dialog. */
    ui->rad_manual->blockSignals(true);
    ui->rad_manual->setChecked(true);
    ui->rad_manual->blockSignals(false);

    ui->rad_scan->setChecked(false);
    ui->txt_uri->setEnabled(true);
    ui->txt_uri->setText(savedIp);
    ui->btn_connect->setFocus();

    QObject::connect(ui->btn_cancel,  &QPushButton::clicked,
                     this, &connectDialog::btn_cancel_clicked, Qt::UniqueConnection);
    QObject::connect(ui->btn_refresh, &QPushButton::clicked,
                     this, &connectDialog::btn_refresh_clicked, Qt::UniqueConnection);
    QObject::connect(ui->btn_connect, &QPushButton::clicked,
                     this, &connectDialog::btn_connect_clicked, Qt::UniqueConnection);
    QObject::connect(ui->rad_scan, &QRadioButton::toggled,
                     this, &connectDialog::rad_scan_toggled, Qt::UniqueConnection);
    QObject::connect(ui->rad_manual, &QRadioButton::toggled,
                     this, &connectDialog::rad_manual_toggled, Qt::UniqueConnection);
    QObject::connect(ui->filter_ip, &QCheckBox::toggled,
                     this, &connectDialog::filter_ip_toggled, Qt::UniqueConnection);
    QObject::connect(ui->filter_usb, &QCheckBox::toggled,
                     this, &connectDialog::filter_usb_toggled, Qt::UniqueConnection);

    /* ReceiverMain will now exec() the dialog and wait for an explicit
     * Connect action from the user. */
    return false;
}

#pragma endregion }

#pragma region Widgets Events  {

/**
 * @brief connectDialog::on_btn_cancel_clicked
 */
void connectDialog::btn_cancel_clicked()
{
    this->close();
}

/**
 * @brief connectDialog::on_btn_refresh_clicked
 */
void connectDialog::btn_refresh_clicked()
{
    ReloadConnectDialog();

    if(ui->rad_scan->isChecked())
        RefreshScanMode();

}

/**
 * @brief connectDialog::on_btn_connect_clicked
 */
void connectDialog::btn_connect_clicked()
{
    /* Accept both "192.168.1.10" and "ip:192.168.1.10" from the user. */
    if (ui->rad_manual->isChecked())
    {
        QString uri = ui->txt_uri->toPlainText().trimmed();
        if (uri.isEmpty())
        {
            ui->txt_context_description->setText("Please enter the board IP address.");
            return;
        }

        if (!uri.startsWith(QStringLiteral("ip:"), Qt::CaseInsensitive))
            uri.prepend(QStringLiteral("ip:"));

        ui->txt_uri->setText(uri);
    }

    /* Always create the context from the value currently shown in the dialog.
     * Do not silently reuse an address created during dialog initialization. */
    struct iio_context *ctx = GetContext();
    if (!ctx)
    {
        char errbuf[256] = {0};
        iio_strerror(errno, errbuf, sizeof(errbuf));
        ui->txt_context_description->setText(
                    QString("Connection failed: %1").arg(QString::fromLocal8Bit(errbuf)));
        qCritical() << "IIO connection failed for"
                    << ui->txt_uri->toPlainText()
                    << ":" << errbuf;
        return;
    }

    // This application is specifically built for the ADRV9009 receiver.
    // Keep the dialog open when the entered IP points to another IIO target.
    if (!iio_context_find_device(ctx, "adrv9009-phy"))
    {
        ui->txt_context_description->setText(
                    "IIO connection succeeded, but adrv9009-phy was not found on this target.");
        qCritical() << "Connected IIO target does not expose adrv9009-phy:"
                    << ui->txt_uri->toPlainText();
        iio_context_destroy(ctx);
        return;
    }

    if (globals::ctx && globals::ctx != ctx)
        iio_context_destroy(globals::ctx);
    globals::ctx = ctx;

    if(ui->rad_scan->isChecked())
    {
        setting.setMode("scan");
    }
    else
    {
        setting.setMode("manual");
        QString ip = ui->txt_uri->toPlainText().trimmed();
        if (ip.startsWith(QStringLiteral("ip:"), Qt::CaseInsensitive))
            ip.remove(0, 3);
        setting.setIp(ip);
    }

    setting.SaveToFile();
    qInfo() << "User confirmed IIO URI:" << ui->txt_uri->toPlainText();
    accept();
}

/**
 * @brief connectDialog::on_rad_scan_toggled
 * @param checked
 */
void connectDialog::rad_scan_toggled(bool checked)
{
    if(!checked)
        return;
    RefreshScanMode();

    ui->filter_ip->setEnabled(checked);
    ui->filter_local->setEnabled(checked);
    ui->filter_usb->setEnabled(checked);

    ui->txt_uri->setEnabled(!checked);
}

/**
 * @brief connectDialog::on_rad_manual_toggled
 * @param checked
 */
void connectDialog::rad_manual_toggled(bool checked)
{
    ui->filter_ip->setEnabled(!checked);
    ui->filter_local->setEnabled(!checked);
    ui->filter_usb->setEnabled(!checked);

    ui->txt_uri->setEnabled(checked);

    if(ui->txt_uri->toPlainText().indexOf("ip")!=-1)
    {
        ReloadConnectDialog();
    }

}

/**
 * @brief connectDialog::on_filter_ip_toggled
 * @param checked
 */
void connectDialog::filter_ip_toggled(bool checked)
{
    if(!checked)
        return;
    RefreshScanMode();
}

/**
 * @brief connectDialog::on_filter_usb_toggled
 * @param checked
 */
void connectDialog::filter_usb_toggled(bool checked)
{
    if(!checked)
        return;
    RefreshScanMode();
}

/**
 * @brief connectDialog::on_cmb_connect_usbd_currentIndexChanged
 * @param index
 */
void connectDialog::on_cmb_connect_usbd_currentIndexChanged(int index)
{
    ReloadConnectDialog();
}

#pragma endregion }

#pragma region Operations {

/**
 * @brief connectDialog::refresh_usb
 */
void connectDialog::RefreshScanMode()
{
    struct iio_scan_context *ctxs;
    struct iio_context_info **info;
    ssize_t ret;
    unsigned int i = 0;
    int index=0;
    gchar *tmp, *tmp1, *pid;
    gchar *buf;
    char *current = nullptr;
    char filter[sizeof("local:ip:usb:")] = {0};
    char *p;
    bool scan = false;
    gchar *active_uri = nullptr;

    for(i = 0; i < 127 ; i++) {
        if (usb_pids[i]) {
            free(usb_pids[i]);
            usb_pids[i] = nullptr;
        }
    }

    p = filter;

    if(ui->filter_local->isChecked())
    {
        p += sprintf(p, "local:");
        scan = true;
    }
    if(ui->filter_usb->isChecked())
    {
        p += sprintf(p, "usb:");
        scan = true;
    }
    if(ui->filter_ip->isChecked())
    {
        p += sprintf(p, "ip:");
        scan = true;
    }

    ui->cmb_connect_usbd->clear();

    ctxs = iio_create_scan_context(filter, 0);
    if (!ctxs)
    {
        qCritical() << "Unable to create IIO scan context";
        ClearDialog();
        return;
    }

    ret = iio_scan_context_get_info_list(ctxs, &info);

    if (ret < 0)
    {
        qInfo()<<"No Info list";
        return;
    }

    for (i = 0; i < (size_t) ret; i++) {
        tmp = strdup(iio_context_info_get_description(info[i]));
        pid = strdup(iio_context_info_get_description(info[i]));

        /* skip the PID/VID or IP Number, example descriptions are:
             * 0456:b673 (Analog Devices Inc. PlutoSDR (ADALM-PLUTO)), serial=104473541196000618001900241f1e6931
             * 192.168.2.1 (Analog Devices PlutoSDR Rev.B (Z7010-AD9363A)), serial=104473541196000618001900241f1e6931
             * 192.168.1.127 (ad7124-4)
             * 192.168.1.120 (AD-FMCOMMS2-EBZ on Xilinx Zynq ZED (armv7l)), serial=00100
             */
        tmp1 = strchr(tmp, '(');
        if (tmp1 && strstr(tmp1, ")), serial=")) {
            /* skip the '(' char */
            tmp1++;
            /* find the serial number */
            if (strstr(tmp1, ")), serial=")) {
                tmp1[strstr(tmp1, ")), serial=") - tmp1 + 1] = 0;
                if (strchr(pid, ' ')) {
                    memmove(strchr(pid, ' ') + 1,
                            strstr(pid, ")), serial=") + strlen(")), serial="),
                            strlen(pid) - (strstr(pid, ")), serial=") - pid -
                                           strlen(")), serial=")));
                } else {
                    printf("error parsing '%s' in refresh_usb\n", pid);
                }
            }
        }
        if (active_pid != -1 && current && !strcmp(pid, current)) {
            index = i;
        }

        usb_pids[i]=pid;

        if (!tmp1)
            tmp1 = tmp;
        buf =(gchar *) malloc(strlen(iio_context_info_get_uri(info[i])) +
                              strlen(tmp1) + 5);
        sprintf(buf, "%s [%s]", tmp1,
                iio_context_info_get_uri(info[i]));
        tmp1 = NULL;

        qInfo()<<"Info Uri => "<<buf;

        ui->cmb_connect_usbd->addItem(buf);

        if (active_uri && !g_strcmp0(active_uri, buf)) {
            index = i;
        }

        free(buf);
        free(tmp);

    }

    if(!i){
        ClearDialog();
    }

}

/**
 * @brief Clear connect dialog
 * @return
 */
void connectDialog::ClearDialog()
{

    if (!ui->rad_manual->isChecked()) {

        /* set - fill in */
        if (ui->rad_scan->isChecked()) {
            if(ReloadConnectDialog()){
                ui->btn_connect->setFocus();
            }
            else
                ui->btn_connect->clearFocus();

        } else {
            /* serial or manual */
            ui->btn_connect->clearFocus();
            ui->cmb_connect_usbd->clearFocus();
        }

    } else {
        /* Unset  - clear */
        ui->txt_fru_info->setText("");
        ui->txt_iio_devices->setText("");
        ui->txt_context_attributes->setText("");
        ui->txt_context_description->setText("");
    }
}

/**
 * @brief connectDialog::get_context
 * @return
 */
iio_context *connectDialog::GetContext()
{
    // Manual Scan Mode
    if (ui->rad_manual->isChecked()) {
        //char *hostname=ui->txt_uri->toPlainText().toLocal8Bit().data();

        QByteArray uri = ui->txt_uri->toPlainText().toLocal8Bit();
        char *hostname = uri.data();
        iio_context *ctx = globals::ctx;

        if (ctx && !g_strcmp0(hostname, iio_context_get_attr_value(ctx, "uri")))
            return ctx;

        return iio_create_context_from_uri(hostname);

        // Network Scan (USB/IP)
    } else if (ui->rad_scan->isChecked()) {
        iio_context *ctx, *ctx2;
        char *uri , *uri2, *uri3;

        if (ui->cmb_connect_usbd->currentIndex() == -1)
            return NULL;

        QByteArray uriData = ui->cmb_connect_usbd->currentText().toLocal8Bit();
        uri = uriData.data();

        if (!strcmp(uri, NO_DEVICES)) {
            return NULL;
        }

        uri2 = uri + strlen(uri);

        while(*uri2 != '[' && uri2 != uri)
            uri2--;

        if (uri2 == uri) {
            //g_free(uri);
            return NULL;
        }
        active_pid = ui->cmb_connect_usbd->currentIndex();

        /* take off the [] */
        uri2++;
        uri2[strlen(uri2)-1] = 0;

        /* are we the same URI? */
        ctx2 = globals::ctx;
        if (ctx2 && !g_strcmp0(uri2, iio_context_get_attr_value(ctx2, "uri"))) {
            //g_free(uri);
            return ctx2;
        }

        ctx = iio_create_context_from_uri(uri2);

        /* If you are looking up ip: with zeroconf, without bonjour installed,
             * try the IP number too
             */
        if (!ctx && strncmp(uri2, "ip:", sizeof("ip:"))) {
            uri3 = strdup(usb_pids[ui->cmb_connect_usbd->currentIndex()]);
            if (uri3) {
                if (strchr(uri3, ' ')) {
                    uri2 = strchr(uri3, ' ');
                    *uri2 = 0;
                    ctx = iio_create_network_context(uri3);
                }
                free(uri3);
            }
        }
        //g_free(uri);
        return ctx;
    } else if (ui->rad_serial_context->isChecked()) {
        iio_context *ctx;
        gchar *port = ui->cmb_port->currentText().toLocal8Bit().data();
        gchar *baud_rate = ui->cmb_baud_rate->currentText().toLocal8Bit().data();
        const gchar *bits8n1 =ui->txt_serial_bit->toPlainText().toLocal8Bit().data();

        /* Size is +3: for ':', ',' and '\0' */
        gchar *result = g_strdup_printf("serial:%s,%s,%s", port, baud_rate, bits8n1);
//        g_free(port);
//        g_free(baud_rate);

        ctx = iio_create_context_from_uri(result);
//        g_free(result);

        if (!ctx && errno == EBUSY &&
                !strcmp("serial", iio_context_get_name(globals::ctx))) {
            return globals::ctx;
        }

        return ctx;

    } else {
        return iio_create_local_context();
    }
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
            text.append (iio_device_get_name(dev));
            text.append ("\r\n");
            text+=tr("%1\n").arg(iio_device_get_name(dev));
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
            globals::ctx=ctx;
        }
    }

    return ctx;
}

#pragma endregion }
