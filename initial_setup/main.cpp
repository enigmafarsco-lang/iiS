// Initial Setup - the small GUI app beside the main project.
//
// The form collects three settings:
//   - the board IP   (192.168.1.10 predefined)
//   - the profile    (100 / 200 / 400 - 200 default)
//   - TX1 on/off     (off default)
//
// Save writes files/initial_setup.ini (the MAIN PROJECT files folder).
// The main software reads that file at startup and asserts the settings.

#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

static QString iniPath()
{
#ifdef PROJECT_FILES_DIR
    const QString dir = QStringLiteral(PROJECT_FILES_DIR);
#else
    const QString dir = QDir::currentPath() + "/files";
#endif
    return QDir(dir).absoluteFilePath(QStringLiteral("initial_setup.ini"));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDialog dlg;
    dlg.setWindowTitle(QStringLiteral("Initial Setup"));

    QLineEdit *ip = new QLineEdit(QStringLiteral("192.168.1.10"));
    ip->setToolTip(QStringLiteral("The board IP address (the software connects here)."));

    QComboBox *profile = new QComboBox;
    profile->addItems(QStringList() << QStringLiteral("100")
                                    << QStringLiteral("200")
                                    << QStringLiteral("400"));
    profile->setCurrentText(QStringLiteral("200")); // 200 is the default
    profile->setToolTip(QStringLiteral("The ADRV9009 profile the software asserts at startup."));

    QCheckBox *tx1 = new QCheckBox(QStringLiteral("on (transmit)"));
    tx1->setChecked(false); // off is the default
    tx1->setToolTip(QStringLiteral("TX1 state asserted at startup - off by default."));

    // The current values of the file, when it exists.
    {
        QFile f(iniPath());
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!f.atEnd()) {
                const QString line = QString::fromUtf8(f.readLine()).trimmed();
                const int eq = line.indexOf(QLatin1Char('='));
                if (eq <= 0)
                    continue;
                const QString key = line.left(eq).trimmed();
                const QString val = line.mid(eq + 1).trimmed();
                if (key == QLatin1String("ip"))
                    ip->setText(val);
                else if (key == QLatin1String("profile"))
                    profile->setCurrentText(val);
                else if (key == QLatin1String("tx1"))
                    tx1->setChecked(val == QLatin1String("1") ||
                                    val.compare(QLatin1String("on"),
                                                Qt::CaseInsensitive) == 0);
            }
        }
    }

    QFormLayout *form = new QFormLayout;
    form->addRow(QStringLiteral("Board IP"), ip);
    form->addRow(QStringLiteral("Profile (100/200/400)"), profile);
    form->addRow(QStringLiteral("TX1"), tx1);

    QPushButton *save = new QPushButton(QStringLiteral("Save"));
    QPushButton *closeBtn = new QPushButton(QStringLiteral("Close"));
    QHBoxLayout *btns = new QHBoxLayout;
    btns->addStretch(1);
    btns->addWidget(save);
    btns->addWidget(closeBtn);

    QLabel *fileLbl = new QLabel(iniPath());
    fileLbl->setStyleSheet(QStringLiteral("color: gray;"));

    QVBoxLayout *root = new QVBoxLayout(&dlg);
    root->addLayout(form);
    root->addWidget(fileLbl);
    root->addLayout(btns);

    QObject::connect(save, &QPushButton::clicked, [&]() {
        QString ipText = ip->text().trimmed();
        if (ipText.isEmpty())
            ipText = QStringLiteral("192.168.1.10");
        const QString pth = iniPath();
        QDir().mkpath(QFileInfo(pth).absolutePath());
        QFile out(pth);
        if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(&dlg, QStringLiteral("Initial Setup"),
                                 QStringLiteral("Cannot write:\n%1").arg(pth));
            return;
        }
        QTextStream w(&out);
        w << "[initial_setup]\n";
        w << "ip=" << ipText << "\n";
        w << "profile=" << profile->currentText() << "\n";
        w << "tx1=" << (tx1->isChecked() ? "1" : "0") << "\n";
        out.close();
        QMessageBox::information(&dlg, QStringLiteral("Initial Setup"),
            QStringLiteral("Saved to:\n%1\n\nThe main software asserts these "
                           "settings at startup.").arg(pth));
    });
    QObject::connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    dlg.show();
    return app.exec();
}
