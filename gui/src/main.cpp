#include "mainwindow.h"

#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QList>

static bool checkFile(QFileInfo info, char *error);

int main(int argc, char *argv[])
{
    char error[2048];

    int noargc = 1;

    QList<QFileInfo> fileList;
    QList<QDir> dirList;
    for (int i = 1; i < argc; i++)
    {
        QFileInfo info(argv[i]);
        if (info.isDir())
        {
            if (info.exists() && info.isExecutable())
            {
                dirList.append(info.absoluteDir());
            }
            else
            {
                fprintf(stderr, "Dir '%s' doesn't exist or can't be accessed; ignored\n", info.filePath().toLocal8Bit().constData());
                fflush(stderr);
            }
        }
        else
        {
            if (checkFile(info, error))
            {
                if (!fileList.contains(info))
                {
                    fileList.append(info);
                }
            }
            else
            {
                fprintf(stderr, "%s\n", error);
                fflush(stderr);
            }
        }
    }

    QStringList nameFilters;
    nameFilters << "*.fit"
                << "*.fits"
                << "*.FIT"
                << "*.FITS";
    QList<QDir>::iterator j;
    for (j = dirList.begin(); j != dirList.end(); ++j)
    {
        QList<QFileInfo> dirFiles = j->entryInfoList(nameFilters, QDir::Files);
        QList<QFileInfo>::iterator k;
        for (k = dirFiles.begin(); k != dirFiles.end(); ++k)
        {
            if (checkFile(*k, error))
            {
                if (!fileList.contains(*k))
                {
                    fileList.append(*k);
                }
            }
            else
            {
                fprintf(stderr, "%s\n", error);
                fflush(stderr);
            }
        }
    }

    QApplication a(noargc, argv);
    QTcpServer server;
    if (server.listen(QHostAddress::LocalHost, 2112))
    {
        if (!fileList.isEmpty())
        {
            MainWindow w(server, fileList);

            w.show();

            return a.exec();
        }
    }
    else
    {
        QTcpSocket socket;
        socket.connectToHost("localhost", 2112);
        if (socket.waitForConnected())
        {
            QDataStream out(&socket);

            qint32 numFiles = fileList.size();
            out << numFiles;

            QList<QFileInfo>::iterator i;
            for (i = fileList.begin(); i != fileList.end(); ++i)
            {
                out << i->absoluteFilePath();
            }

            socket.flush();
            socket.close();
        }
    }

    return 1;
}

bool checkFile(QFileInfo info, char *error)
{
    const char magic[] = {'S', 'I', 'M', 'P', 'L', 'E'};

    if (info.exists())
    {
        QFile f(info.absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly))
        {
            sprintf(error, "File '%s' is not readable; ignored", info.filePath().toLocal8Bit().constData());
            return false;
        }
        else
        {
            char fileMagic[6];
            if (f.read(fileMagic, 6) != 6)
            {
                sprintf(error, "File '%s': small read short or failed verifying magic; ignored", info.filePath().toLocal8Bit().constData());
                f.close();
                return false;
            }
            else
            {
                for (int i = 0; i < 6; i++)
                {
                    if (magic[i] != fileMagic[i])
                    {
                        sprintf(error, "File '%s': not a FITS file (bad magic); ignored", info.filePath().toLocal8Bit().constData());
                        f.close();
                        return false;
                    }
                }
            }

            f.close();
        }
    }
    else
    {
        sprintf(error, "File '%s' doesn't exist; ignored", info.filePath().toLocal8Bit().constData());
        return false;
    }

    return true;
}
