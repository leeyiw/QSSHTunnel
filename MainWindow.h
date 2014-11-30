#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
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
#include <QSystemTrayIcon>
#include <QTemporaryFile>
#include <QWidget>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void connectBtnClicked();
	void sshReadyReadStdout();
    void sshReadyReadStderr();

//    void aboutClicked();

    void systemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void operationActionTriggered();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void prepareMenuBar();
    void prepareTrayIcon();
    void loadSettings();

    void connectSSH();
    void disconnectSSH();

    enum CurrentState { NotConnected, Connecting, Connected, Disconnecting };
    CurrentState currentState;
    void setCurrentState(CurrentState state);

    QProcess *sshProcess;
    QTemporaryFile *sshAskPassFile;
    QSettings settings;

    /* Menus and Actions */
//    QMenu *helpMenu;
//    QAction *aboutAction;

    /* Tray icon, menu and actions */
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *statusAction;
    QAction *operationAction;

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
