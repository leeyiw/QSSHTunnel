#ifndef MYTEMPORARYFILE_H
#define MYTEMPORARYFILE_H

#include <QTemporaryFile>
#include <QFile>

class MyTemporaryFile
{
public:
    MyTemporaryFile();
    ~MyTemporaryFile();

    bool open();
    void close();
    bool setPermissions(QFileDevice::Permissions permissions);
    QFileDevice::Permissions permissions() const;
    QString fileName() const;

    QTemporaryFile *getFile() const;

private:
    QTemporaryFile *tmpFile;
    QString tmpFileName;
    bool autoRemove;
};

#endif // MYTEMPORARYFILE_H
