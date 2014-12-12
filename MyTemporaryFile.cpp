#include "MyTemporaryFile.h"

MyTemporaryFile::MyTemporaryFile()
{
    tmpFile = new QTemporaryFile();
    /* take control of autoRemove, then QTemporaryFile never do autoRemove */
    autoRemove = tmpFile->autoRemove();
    tmpFile->setAutoRemove(false);
}

MyTemporaryFile::~MyTemporaryFile()
{
    /* if autoRemove is set, delete the file */
    if (autoRemove) {
        QFile::remove(tmpFileName);
    }
}

bool
MyTemporaryFile::open()
{
    bool ret;
    ret = tmpFile->open();
    tmpFileName = tmpFile->fileName();
    return ret;
}

void
MyTemporaryFile::close()
{
    /* delete the QTemporaryFile object to release file handle */
    delete tmpFile;
}

bool
MyTemporaryFile::setPermissions(QFileDevice::Permissions permissions)
{
    return tmpFile->setPermissions(permissions);
}

QFileDevice::Permissions
MyTemporaryFile::permissions() const
{
    return tmpFile->permissions();
}

QString
MyTemporaryFile::fileName() const
{
    return tmpFileName;
}

QTemporaryFile *
MyTemporaryFile::getFile() const
{
    return tmpFile;
}
