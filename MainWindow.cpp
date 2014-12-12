#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      currentState(NotConnected),
      sshProcess(NULL),
      sshAskPassFile(NULL),
      timer(NULL)
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

    statusLabel = new QLabel();
    layout->addRow(tr("Status:"), statusLabel);

    connectBtn = new QPushButton();
    connect(connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnClicked()));
    layout->addRow(connectBtn);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    prepareMenuBar();
#ifndef Q_OS_LINUX
    prepareTrayIcon();
#endif

    loadSettings();

    setCurrentState(NotConnected);
}

MainWindow::~MainWindow()
{

}

void
MainWindow::connectBtnClicked()
{
    if (!this->validateForm()) {
        return;
    }

    connectBtn->setDisabled(true);
    if (currentState == NotConnected) {
        connectSSH();
    } else {
        disconnectSSH();
    }
}

void
MainWindow::operationActionTriggered()
{
    if (!this->validateForm()) {
        return;
    }

    if (currentState == NotConnected) {
        connectSSH();
    } else if (currentState == Connected) {
        disconnectSSH();
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
        /* Connect failed, maybe incorrect username or password */
        setCurrentState(NotConnected);
        statusLabel->setText(tr("Connect failed (permission denied)"));
        if (sshAskPassFile != NULL) {
            delete sshAskPassFile;
            sshAskPassFile = NULL;
        }
    } else if (data.contains("Operation timed out")) {
        /* Connect failed, maybe incorrect IP/port */
        setCurrentState(NotConnected);
        statusLabel->setText(tr("Connect failed (Operation timed out)"));
    } else if (data.contains("Entering interactive session.")) {
        /* Connect success */
        setCurrentState(Connected);
    } else {
        /* If output is not key information, then return, don't remove
         * sshAskPassFile */
        return;
    }
    if (sshAskPassFile != NULL) {
        delete sshAskPassFile;
        sshAskPassFile = NULL;
    }
}

void
MainWindow::updateTime()
{
    QTime t(0, 0, 0);
    t = t.addMSecs(elapsedTimer.elapsed());
    QString status = tr("Connected (%1)").arg(t.toString("hh:mm:ss"));
    statusLabel->setText(status);
#ifndef Q_OS_LINUX
    statusAction->setText(status);
#endif
}

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
}

#ifndef Q_OS_LINUX
void
MainWindow::prepareTrayIcon()
{
    trayIcon = new QSystemTrayIcon(QIcon(":/images/images/icon_16x16@2x.png"),
                                   this);
    trayMenu = new QMenu(this);

    /* Add status and operation action */
    statusAction = new QAction(this);
    statusAction->setDisabled(true);
    trayMenu->addAction(statusAction);
    operationAction = new QAction(this);
    connect(operationAction, SIGNAL(triggered()),
            this, SLOT(operationActionTriggered()));
    trayMenu->addAction(operationAction);

    /* Separator */
    trayMenu->addSeparator();

    /* Quit action */
    trayMenu->addAction(tr("Quit %1").arg(qApp->applicationName()),
                        qApp, SLOT(quit()));

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}
#endif

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
    socksServerAddrEdit->setText(settings.value("socks_server/addr",
                                                "127.0.0.1").toString());
    socksServerPortEdit->setText(settings.value("socks_server/port",
                                                "7070").toString());
}

bool
MainWindow::validateForm()
{
    if (sshServerAddrEdit->text().length() == 0) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Invalid SSH server address"));
        return false;
    }
    if (sshServerPortEdit->text().length() == 0) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Invalid SSH server address"));
        return false;
    }
    if (usernameEdit->text().length() == 0) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Invalid SSH username"));
        return false;
    }
    if (passwordEdit->text().length() == 0) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Invalid SSH password"));
        return false;
    }
    if (socksServerAddrEdit->text().length() == 0) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Invalid SOCKS server address"));
        return false;
    }
    if (socksServerPortEdit->text().length() == 0) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Invalid SOCKS server address"));
        return false;
    }
    return true;
}

void
MainWindow::connectSSH()
{
    setCurrentState(Connecting);

    /* Create SSH process object */
    sshProcess = new QProcess(this);
    connect(sshProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(sshReadyReadStdout()));
    connect(sshProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(sshReadyReadStderr()));

    /* Generate SSH_ASKPASS file with right permission and content */
    sshAskPassFile = new MyTemporaryFile();
    if (!sshAskPassFile->open()) {
        setCurrentState(NotConnected);
        statusLabel->setText(tr("Connect failed (SSH_ASKPASS error)"));
        return;
    }
    QFile::Permissions perm = sshAskPassFile->permissions();
    perm |= QFile::ExeOwner | QFile::ExeUser;
    sshAskPassFile->setPermissions(perm);
    QTextStream out(sshAskPassFile->getFile());
    out << "#!/usr/bin/env bash\n";
    out << "echo '" << passwordEdit->text() << "'\n";
    sshAskPassFile->close();

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
    arguments << "-o" << "ConnectTimeout=10";
    arguments << QString("%1@%2").arg(usernameEdit->text(),
                                      sshServerAddrEdit->text());
    sshProcess->start("ssh", arguments);
    if (!sshProcess->waitForStarted(1000)) {
        setCurrentState(NotConnected);
        statusLabel->setText(tr("SSH start failed"));
        return;
    }
}

void
MainWindow::disconnectSSH()
{
    setCurrentState(Disconnecting);

    sshProcess->kill();
    delete sshProcess;
    sshProcess = NULL;

    setCurrentState(NotConnected);
}

void
MainWindow::setCurrentState(CurrentState state)
{
    currentState = state;

    if (currentState != Connected && timer != NULL) {
        delete timer;
        timer = NULL;
    }

    if (currentState == NotConnected) {
        /* Enable user input area */
        sshServerAddrEdit->setEnabled(true);
        sshServerPortEdit->setEnabled(true);
        usernameEdit->setEnabled(true);
        passwordEdit->setEnabled(true);
        remberPasswordCheckBox->setEnabled(true);
        socksServerAddrEdit->setEnabled(true);
        socksServerPortEdit->setEnabled(true);

        statusLabel->setText(tr("Not connected"));
        connectBtn->setText(tr("Connect"));
        connectBtn->setEnabled(true);
#ifndef Q_OS_LINUX
        statusAction->setText(tr("Not connected"));
        operationAction->setText(tr("Connect"));
        operationAction->setVisible(true);
#endif
    } else if (currentState == Connecting) {
        /* Disable user input area */
        sshServerAddrEdit->setDisabled(true);
        sshServerPortEdit->setDisabled(true);
        usernameEdit->setDisabled(true);
        passwordEdit->setDisabled(true);
        remberPasswordCheckBox->setDisabled(true);
        socksServerAddrEdit->setDisabled(true);
        socksServerPortEdit->setDisabled(true);

        statusLabel->setText(tr("Connecting..."));
        connectBtn->setEnabled(false);
#ifndef Q_OS_LINUX
        statusAction->setText(tr("Connecting..."));
        operationAction->setVisible(false);
#endif
    } else if (currentState == Connected) {
        /* Start timer to update elapsed time in status label */
        elapsedTimer.restart();
        updateTime();
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
        timer->start(1000);

        connectBtn->setText(tr("Disconnect"));
        connectBtn->setEnabled(true);
#ifndef Q_OS_LINUX
        operationAction->setText(tr("Disconnect"));
        operationAction->setVisible(true);
#endif
    } else if (currentState == Disconnecting) {
        statusLabel->setText(tr("Disconnecting..."));
        connectBtn->setEnabled(false);
#ifndef Q_OS_LINUX
        statusAction->setText(tr("Disconnecting..."));
        operationAction->setVisible(false);
#endif
    }
}
