#include "filetransferworker.h"
#include <QDebug>
#include <libssh2.h>

FileTransferWorker::FileTransferWorker(LIBSSH2_SFTP *sftpSession, LIBSSH2_SESSION *session, QObject *parent)
    : QObject(parent), sftpSession(sftpSession), session(session)
{
}

void FileTransferWorker::setUploadParams(const QString &localPath, const QString &remotePath)
{
    this->localPath = localPath;
    this->remotePath = remotePath;
}

void FileTransferWorker::setDownloadParams(const QString &remotePath, const QString &localPath)
{
    this->localPath = localPath;
    this->remotePath = remotePath;
}

void FileTransferWorker::uploadFile()
{
    if (!sftpSession || !session) {
        emit errorOccurred("Invalid SFTP session or LIBSSH2 session instance.");
        return;
    }

    QFile localFile(localPath);
    if (!localFile.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Unable to open local file: " + localPath);
        qDebug() << "Failed to open local file:" << localPath;
        return;
    }

    qint64 totalBytes = localFile.size();
    qint64 bytesTransferred = 0;
    qDebug() << "Local file size:" << totalBytes;

    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(
        sftpSession, remotePath.toUtf8().data(),
        LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
        LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR | LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);

    if (!sftp_handle) {
        char *errmsg;
        libssh2_session_last_error(session, &errmsg, nullptr, 0);
        emit errorOccurred("Unable to open remote file: " + remotePath + ". Error: " + QString(errmsg));
        qDebug() << "Failed to open remote file:" << remotePath << ". Error:" << errmsg;
        return;
    }

    char buffer[1024];
    ssize_t bytesRead, bytesWritten;

    while ((bytesRead = localFile.read(buffer, sizeof(buffer))) > 0) {
        bytesWritten = libssh2_sftp_write(sftp_handle, buffer, bytesRead);
        if (bytesWritten < 0) {
            char *errmsg;
            libssh2_session_last_error(session, &errmsg, nullptr, 0);
            emit errorOccurred("Error writing to remote file. Error: " + QString(errmsg));
            qDebug() << "Error writing to remote file. Bytes attempted:" << bytesRead << ". Error:" << errmsg;
            libssh2_sftp_close(sftp_handle);
            return;
        }
        bytesTransferred += bytesWritten;
        emit progressUpdated(bytesTransferred, totalBytes);
    }

    localFile.close();
    libssh2_sftp_close(sftp_handle);
    emit transferCompleted();
    qDebug() << "File upload completed successfully.";    
}

void FileTransferWorker::downloadFile()
{
    if (!sftpSession || !session) {
        emit errorOccurred("Invalid SFTP session or LIBSSH2 session instance.");
        return;
    }

    QFile localFile(localPath);
    if (!localFile.open(QIODevice::WriteOnly)) {
        emit errorOccurred("Unable to open local file for writing: " + localPath);
        qDebug() << "Failed to open local file:" << localPath;
        return;
    }

    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(
        sftpSession, remotePath.toUtf8().data(),
        LIBSSH2_FXF_READ, 0);

    if (!sftp_handle) {
        char *errmsg;
        libssh2_session_last_error(session, &errmsg, nullptr, 0);
        emit errorOccurred("Unable to open remote file: " + remotePath + ". Error: " + QString(errmsg));
        qDebug() << "Failed to open remote file:" << remotePath << ". Error:" << errmsg;
        return;
    }

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    if (libssh2_sftp_fstat(sftp_handle, &attrs) != 0) {
        char *errmsg;
        libssh2_session_last_error(session, &errmsg, nullptr, 0);
        emit errorOccurred("Failed to retrieve file attributes for: " + remotePath + ". Error: " + QString(errmsg));
        qDebug() << "Failed to retrieve file attributes for:" << remotePath << ". Error:" << errmsg;
        libssh2_sftp_close(sftp_handle);
        return;
    }

    qint64 totalBytes = attrs.filesize;
    qint64 bytesTransferred = 0;
    qDebug() << "Remote file size:" << totalBytes;

    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = libssh2_sftp_read(sftp_handle, buffer, sizeof(buffer))) > 0) {
        localFile.write(buffer, bytesRead);
        bytesTransferred += bytesRead;
        emit progressUpdated(bytesTransferred, totalBytes);
    }

    if (bytesRead < 0) {
        char *errmsg;
        libssh2_session_last_error(session, &errmsg, nullptr, 0);
        emit errorOccurred("Error reading from remote file. Error: " + QString(errmsg));
        qDebug() << "Error reading from remote file. Error:" << errmsg;
    }

    localFile.close();
    libssh2_sftp_close(sftp_handle);
    emit transferCompleted();
    qDebug() << "File download completed successfully.";    
}
