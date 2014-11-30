#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QMainWindow>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QProcess>
#include <QTemporaryFile>
#include <QSettings>

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

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void loadSettings();

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

    QProcess *sshProcess;
    QTemporaryFile *sshAskPassFile;
    QSettings settings;
};

#endif // MAINWINDOW_H
