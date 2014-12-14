#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDebug>
#include <QElapsedTimer>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QtGlobal>
#include <QTime>
#include <QTimer>
#include <QWidget>

#include "MyTemporaryFile.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void connectBtnClicked();
    void operationActionTriggered();

	void sshReadyReadStdout();
    void sshReadyReadStderr();

    void updateTime();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void prepareMenuBar();
#ifndef Q_OS_LINUX
    void prepareTrayIcon();
#endif
    void loadSettings();

    bool validateForm();
    void connectSSH();
    void disconnectSSH();
    void updateIcon();

    enum CurrentState { NotConnected, Connecting, Connected, Disconnecting };
    CurrentState currentState;
    void setCurrentState(CurrentState state);

    QProcess *sshProcess;
    MyTemporaryFile *sshAskPassFile;
    QSettings settings;
    QElapsedTimer elapsedTimer;
    QTimer *timer;

#ifndef Q_OS_LINUX
    /* Tray icon, menu and actions */
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *statusAction;
    QAction *operationAction;
#endif

    /* Base layout */
	QFormLayout *layout;
    /* SSH Server field */
	QHBoxLayout *sshServerLayout;
	QLineEdit *sshServerAddrEdit, *sshServerPortEdit;
	QLabel *sshServerColon;
    /* Username field */
	QLineEdit *usernameEdit;
    /* Password field */
    QHBoxLayout *passwordLayout;
	QLineEdit *passwordEdit;
    QCheckBox *remberPasswordCheckBox;
    /* SOCKS Server field */
	QHBoxLayout *socksServerLayout;
	QLineEdit *socksServerAddrEdit, *socksServerPortEdit;
	QLabel *socksServerColon;
    /* Status field */
	QLabel *statusLabel;
    /* Connect button */
    QPushButton *connectBtn;
};

#endif // MAINWINDOW_H
