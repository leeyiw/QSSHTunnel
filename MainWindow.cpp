#include <QStringList>
#include <QDebug>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      sshProcess(NULL),
      sshAskPassFile(NULL)
{
    setWindowTitle(qApp->applicationName());

    layout = new QFormLayout();

    sshServerLayout = new QHBoxLayout();
    sshServerAddrEdit = new QLineEdit();
    sshServerLayout->addWidget(sshServerAddrEdit);
    sshServerColon = new QLabel(":");
    sshServerLayout->addWidget(sshServerColon);
    sshServerPortEdit = new QLineEdit();
    sshServerPortEdit->setMaxLength(5);
    sshServerPortEdit->setFixedWidth(50);
    sshServerLayout->addWidget(sshServerPortEdit);
    layout->addRow(tr("SSH Server:"), sshServerLayout);

    usernameEdit = new QLineEdit();
    layout->addRow(tr("Username:"), usernameEdit);

    passwordLayout = new QHBoxLayout();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordLayout->addWidget(passwordEdit);
    remberPasswordCheckBox = new QCheckBox(tr("Rember"));
    passwordLayout->addWidget(remberPasswordCheckBox);
    layout->addRow(tr("Password:"), passwordLayout);

    socksServerLayout = new QHBoxLayout();
    socksServerAddrEdit = new QLineEdit();
    socksServerLayout->addWidget(socksServerAddrEdit);
    socksServerColon = new QLabel(":");
    socksServerLayout->addWidget(socksServerColon);
    socksServerPortEdit = new QLineEdit();
    socksServerPortEdit->setMaxLength(5);
    socksServerPortEdit->setFixedWidth(50);
    socksServerLayout->addWidget(socksServerPortEdit);
    layout->addRow(tr("SOCKS Server:"), socksServerLayout);

    statusLabel = new QLabel(tr("Not connected"));
    layout->addRow(tr("Status:"), statusLabel);

    connectBtn = new QPushButton(tr("Connect"));
    connect(connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnClicked()));
    layout->addRow(connectBtn);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    prepareMenuBar();
    prepareTrayIcon();

    loadSettings();
}

MainWindow::~MainWindow()
{

}

void
MainWindow::connectBtnClicked()
{
    connectBtn->setDisabled(true);
    if (sshProcess == NULL) {
        statusLabel->setText(tr("Connecting..."));

        /* Create SSH process object */
        sshProcess = new QProcess(this);
        connect(sshProcess, SIGNAL(readyReadStandardOutput()),
                this, SLOT(sshReadyReadStdout()));
        connect(sshProcess, SIGNAL(readyReadStandardError()),
                this, SLOT(sshReadyReadStderr()));

        /* Generate SSH_ASKPASS file with right permission and content */
        sshAskPassFile = new QTemporaryFile();
        if (!sshAskPassFile->open()) {
            statusLabel->setText(tr("Connect failed (SSH_ASKPASS error)"));
            connectBtn->setEnabled(true);
            return;
        }
        QFile::Permissions perm = sshAskPassFile->permissions();
        qDebug() << sshAskPassFile->fileName();
        perm |= QFile::ExeOwner | QFile::ExeUser;
        sshAskPassFile->setPermissions(perm);
        QTextStream out(sshAskPassFile);
        out << "#!/usr/bin/env bash\n";
        out << "echo '" << passwordEdit->text() << "'\n";

        /* Set SSH_ASKPASS enviroment variable */
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("SSH_ASKPASS", sshAskPassFile->fileName());
        sshProcess->setProcessEnvironment(env);

        /* Assemble arguments and start SSH process */
        QStringList arguments;
        arguments << "-qTnN";
        arguments << "-v";
        arguments << "-D" << QString("%1:%2").arg(socksServerAddrEdit->text(),
                                                  socksServerPortEdit->text());
        arguments << "-p" << sshServerPortEdit->text();
        arguments << QString("%1@%2").arg(usernameEdit->text(),
                                          sshServerAddrEdit->text());
        sshProcess->start("ssh", arguments);
        if (!sshProcess->waitForStarted(1000)) {
            statusLabel->setText(tr("SSH start failed"));
            connectBtn->setEnabled(true);
            return;
        }
    } else {
        statusLabel->setText(tr("Disconnecting..."));
        sshProcess->kill();
        delete sshProcess;
        sshProcess = NULL;
        statusLabel->setText(tr("Not connected"));
        connectBtn->setText(tr("Connect"));
        connectBtn->setEnabled(true);
    }
}

void
MainWindow::sshReadyReadStdout()
{
    QString data(sshProcess->readAllStandardOutput());
    qDebug() << "stdout:" << data;
}

void
MainWindow::sshReadyReadStderr()
{
    QString data(sshProcess->readAllStandardError());
    qDebug() << "stderr:" << data;

    if (data.contains("Permission denied")) {
        /* Connect failed */
        statusLabel->setText(tr("Connect failed (permission denied)"));
        connectBtn->setEnabled(true);

        if (sshAskPassFile != NULL) {
            delete sshAskPassFile;
            sshAskPassFile = NULL;
        }
    } else if (data.contains("Entering interactive session.")) {
        /* Connect success */
        statusLabel->setText(tr("Connected"));
        connectBtn->setText(tr("Disconnect"));
        connectBtn->setEnabled(true);

        if (sshAskPassFile != NULL) {
            delete sshAskPassFile;
            sshAskPassFile = NULL;
        }
    }
}

//void
//MainWindow::aboutClicked()
//{
//    QString title = tr("About") + " " + qApp->applicationName();
//    QString text = qApp->applicationName() + " is a open source software";
//    QMessageBox::about(this, title, text);
//}

void
MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("ssh_server/addr", sshServerAddrEdit->text());
    settings.setValue("ssh_server/port", sshServerPortEdit->text());
    settings.setValue("username", usernameEdit->text());
    if (remberPasswordCheckBox->isChecked()) {
        settings.setValue("password/value", passwordEdit->text());
        settings.setValue("password/rember",
                          remberPasswordCheckBox->isChecked());
    } else {
        settings.remove("password");
    }
    settings.setValue("socks_server/addr", socksServerAddrEdit->text());
    settings.setValue("socks_server/port", socksServerPortEdit->text());
    event->accept();
}

void
MainWindow::prepareMenuBar()
{
    /* Set menu bar */
//    helpMenu = new QMenu(tr("Help"));
//    helpMenu->addAction(tr("About"), this, SLOT(aboutClicked()));
//    this->menuBar()->addMenu(hnielpMenu);
}

void
MainWindow::prepareTrayIcon()
{
    trayIcon = new QSystemTrayIcon(QIcon(":/images/images/icon_16x16@2x.png"),
                                   this);
    trayMenu = new QMenu(this);
    trayMenu->addAction(tr("Quit %1").arg(qApp->applicationName()),
                        qApp, SLOT(quit()));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

void
MainWindow::loadSettings()
{
    sshServerAddrEdit->setText(settings.value("ssh_server/addr").toString());
    sshServerPortEdit->setText(settings.value("ssh_server/port",
                                              "22").toString());
    usernameEdit->setText(settings.value("username").toString());
    passwordEdit->setText(settings.value("password/value").toString());
    remberPasswordCheckBox->setChecked(settings.value("password/rember",
                                                      false).toBool());
    socksServerAddrEdit->setText(settings.value("socks_server/addr").toString());
    socksServerPortEdit->setText(settings.value("socks_server/port",
                                                "7070").toString());
}
