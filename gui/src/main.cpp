#include <QtGlobal>
#include <QApplication>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>

#include "image.h"
#include "imagefilelistitem.h"
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    char error[2048];

    int noargc = 1;

    QList<ImageFileListItem> fileList;
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
                fprintf(stderr, "Dir '%s' doesn't exist or can't be accessed; ignored\n",
                        qPrintable(info.filePath()));
                fflush(stderr);
            }
        }
        else
        {
            QByteArray ba = info.absoluteFilePath().toLocal8Bit();
            const char* absPath = ba.data();
            ELS::Image::FileType fileType = ELS::Image::isSupportedFile(absPath, error);
            if (fileType != ELS::Image::FT_UNKNOWN)
            {
                ImageFileListItem item(absPath, fileType, true);
                if (!fileList.contains(item))
                {
                    fileList.append(item);
                }
            }
            else
            {
                fprintf(stderr, "%s\n", error);
                fflush(stderr);
            }
        }
    }

    QList<QDir>::iterator j;
    for (j = dirList.begin(); j != dirList.end(); ++j)
    {
        QList<QFileInfo> dirFiles = j->entryInfoList(QDir::Files);
        QList<QFileInfo>::iterator k;
        for (k = dirFiles.begin(); k != dirFiles.end(); ++k)
        {
            QByteArray ba = k->absoluteFilePath().toLocal8Bit();
            const char* absPath = ba.data();
            ELS::Image::FileType fileType = ELS::Image::isSupportedFile(absPath, error);
            if (fileType != ELS::Image::FT_UNKNOWN)
            {
                ImageFileListItem item(absPath, fileType, true);
                if (!fileList.contains(item))
                {
                    fileList.append(item);
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

            QList<ImageFileListItem>::iterator i;
            for (i = fileList.begin(); i != fileList.end(); ++i)
            {
                out << *i;
            }

            socket.flush();
            socket.close();
        }
    }

    return 1;
}
