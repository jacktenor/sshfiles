// filetransferworker.cpp
#include "filetransferworker.h"

FileTransferWorker::FileTransferWorker(LIBSSH2_SFTP *sftpSession, QObject *parent)
    : QObject(parent), sftpSession(sftpSession)
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
    QFile localFile(localPath);
    if (!localFile.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Unable to open local file: " + localPath);
        return;
    }

    // Get the total size of the file for progress calculation
    qint64 totalBytes = localFile.size();
    qint64 bytesTransferred = 0;

    // Open the remote file on the SFTP server
    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(sftpSession, remotePath.toUtf8().data(),
                                                         LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
                                                         LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
                                                             LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);
    if (!sftp_handle) {
        emit errorOccurred("Unable to open remote file: " + remotePath);
        return;
    }

    // Buffer for reading the local file and writing to the remote file
    char buffer[1024];
    ssize_t bytesRead, bytesWritten;

    // Read the local file and write to the remote file in chunks
    while ((bytesRead = localFile.read(buffer, sizeof(buffer))) > 0) {
        bytesWritten = libssh2_sftp_write(sftp_handle, buffer, bytesRead);
        if (bytesWritten < 0) {
            emit errorOccurred("Error writing to remote file.");
            libssh2_sftp_close(sftp_handle);
            return;
        }
        bytesTransferred += bytesWritten;

        // Emit the progressUpdated signal after each chunk
        emit progressUpdated(bytesTransferred, totalBytes);
    }

    // Close the files and signal completion
    localFile.close();
    libssh2_sftp_close(sftp_handle);

    emit transferCompleted();
}

void FileTransferWorker::downloadFile()
{
    QFile localFile(localPath);
    if (!localFile.open(QIODevice::WriteOnly)) {
        emit errorOccurred("Unable to open local file for writing: " + localPath);
        return;
    }

    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(sftpSession, remotePath.toUtf8().data(), LIBSSH2_FXF_READ, 0);
    if (!sftp_handle) {
        emit errorOccurred("Unable to open remote file: " + remotePath);
        return;
    }

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    libssh2_sftp_fstat(sftp_handle, &attrs); // Get the file size
    qint64 totalBytes = attrs.filesize;
    qint64 bytesTransferred = 0;

    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = libssh2_sftp_read(sftp_handle, buffer, sizeof(buffer))) > 0) {
        localFile.write(buffer, bytesRead);
        bytesTransferred += bytesRead;
        emit progressUpdated(bytesTransferred, totalBytes);
    }

    localFile.close();
    libssh2_sftp_close(sftp_handle);
    emit transferCompleted();
}
