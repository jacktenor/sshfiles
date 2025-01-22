// filetransferworker.h
#ifndef FILETRANSFERWORKER_H
#define FILETRANSFERWORKER_H

#include <QObject>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <QFile>
#include <QString>

class FileTransferWorker : public QObject
{
    Q_OBJECT

public:
    explicit FileTransferWorker(LIBSSH2_SFTP *sftpSession, LIBSSH2_SESSION *session, QObject *parent = nullptr);
    void setUploadParams(const QString &localPath, const QString &remotePath);
    void setDownloadParams(const QString &remotePath, const QString &localPath);
public slots:
    void uploadFile();
    void downloadFile();

signals:
    void transferCompleted();
    void errorOccurred(const QString &error);
    void progressUpdated(qint64 bytesTransferred, qint64 totalBytes);

private:
    LIBSSH2_SFTP *sftpSession;
    LIBSSH2_SESSION *session;  // Add this declaration
    QString localPath;
    QString remotePath;
};

#endif // FILETRANSFERWORKER_H
