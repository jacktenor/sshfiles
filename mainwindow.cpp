#include "mainwindow.h"
#include "filetransferworker.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDir>
#include <QFileDialog>
#include <QThread>
#include <QSettings>
#include <QDebug>

// Constructor of MainWindow class
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), tcpSocket(new QTcpSocket(this)), session(nullptr), sftp_session(nullptr), settings("Sativa.inc", "SSHFiletransfer")
{
    ui->setupUi(this);

    // Create and set the QGraphicsScene
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Debugging output
    qDebug() << "graphicsView initialized and scene set.";

    // Load an image from resources (check the file path in the .qrc file)
    QPixmap pixmap(":/cat.png");
    if (pixmap.isNull()) {
        qDebug() << "Failed to load image ':/cat.png'. Ensure it's in the resources.";
    } else {
        QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
        scene->addItem(item);
        qDebug() << "Image successfully loaded and added to the scene.";
    }

    // Initialize UI components and connections as needed
    ui->uploadProgressBar->setValue(0);
    ui->downloadProgressBar->setValue(0);

    ui->remoteFileTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->ipComboBox->setEditable(true);
    ui->usernameComboBox->setEditable(true);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Load login history on application start
    loadLoginDetails();

    // Setup context menu, signal connections, etc.
    connect(ui->remoteFileTree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::on_remoteTreeContextMenuRequested);
    connect(ui->ipComboBox, &QComboBox::editTextChanged, this, &MainWindow::updatePasswordField);
    connect(ui->usernameComboBox, &QComboBox::editTextChanged, this, &MainWindow::updatePasswordField);
    connect(ui->remoteFileTree, &QTreeWidget::itemClicked, this, &MainWindow::on_itemClicked);
    connect(ui->localFileTree, &QTreeWidget::itemClicked, this, &MainWindow::on_localItemClicked);
}

// Destructor of MainWindow class
MainWindow::~MainWindow()
{
    delete ui;
    if (sftp_session) {
        libssh2_sftp_shutdown(sftp_session);
    }
    if (session) {
        libssh2_session_disconnect(session, "Normal Shutdown");
        libssh2_session_free(session);
    }
    libssh2_exit();
}
void MainWindow::on_connectButton_clicked()
{
    QString host = ui->ipComboBox->currentText();
    QString user = ui->usernameComboBox->currentText();
    QString password = ui->passwordLineEdit->text();

    // Save login details, including the password if the checkbox is checked
    saveLoginDetails(host, user, ui->savePasswordCheckBox->isChecked() ? password : "");

    connectToServer(host, user, password);
}

void MainWindow::on_itemClicked(QTreeWidgetItem *item)
{

    QString path = item->data(0, Qt::UserRole).toString();  // Full path
    bool isDirectory = item->data(0, Qt::UserRole + 1).toBool();

    if (isDirectory) {
        listRemoteFiles(path);  // If it's a directory, list its contents
    } else {
        if (QMessageBox::Yes == QMessageBox::question(this, "Download", "Download file: " + path + "?")) {
            QString downloadPath = QDir::homePath() + "/Downloads/" + item->text(0);
            downloadFile(path, downloadPath);  // Start the download
        }
    }
}

void MainWindow::on_localItemClicked(QTreeWidgetItem *item)
{
    QString path = item->data(0, Qt::UserRole).toString();  // Full path
    bool isDirectory = item->data(0, Qt::UserRole + 1).toBool();

    if (isDirectory) {
        listLocalFiles(path);  // Navigate into the local directory
    } else {
        if (QMessageBox::Yes == QMessageBox::question(this, "Upload", "Upload file: " + path + " to ~/Downloads on the remote server?")) {
            QString remoteUploadPath = "/home/" + ui->usernameComboBox->currentText() + "/Downloads/" + item->text(0);
            uploadFile(path, remoteUploadPath);  // Start the upload
        }
    }
}

void MainWindow::saveLoginDetails(const QString &host, const QString &user, const QString &password)
{
    settings.beginGroup(host + "/" + user);  // Store details by host/username
    settings.setValue("password", password);
    settings.endGroup();

    // Store the IP and username in history
    QStringList ipHistory = settings.value("ipHistory").toStringList();
    if (!ipHistory.contains(host)) {
        ipHistory.append(host);
        settings.setValue("ipHistory", ipHistory);
    }

    QStringList userHistory = settings.value("userHistory").toStringList();
    if (!userHistory.contains(user)) {
        userHistory.append(user);
        settings.setValue("userHistory", userHistory);
    }
}

void MainWindow::loadLoginDetails()
{
    // Load IP history and fill the combobox
    QStringList ipHistory = settings.value("ipHistory").toStringList();
    ui->ipComboBox->addItems(ipHistory);

    // Load username history and fill the combobox
    QStringList userHistory = settings.value("userHistory").toStringList();
    ui->usernameComboBox->addItems(userHistory);
}

void MainWindow::updatePasswordField()
{
    QString host = ui->ipComboBox->currentText();
    QString user = ui->usernameComboBox->currentText();

    // Try to retrieve the password based on the host and username
    settings.beginGroup(host + "/" + user);
#include <QGraphicsScene>       // To create the graphics scene
    QString savedPassword = settings.value("password").toString();
    settings.endGroup();

    // Auto-fill the password if available
    ui->passwordLineEdit->setText(savedPassword);
}

// Function to handle connection to the SSH server
void MainWindow::connectToServer(const QString &host, const QString &user, const QString &password)
{
    tcpSocket->connectToHost(host, 22);  // Port 22 is typically used for SSH

    if (!tcpSocket->waitForConnected(5000)) {  // Wait up to 5 seconds for the connection
        QMessageBox::warning(this, "Connection Failed", "Unable to connect to SSH server.");
        return;
    }

    // Initialize libssh2
    libssh2_init(0);
    session = libssh2_session_init();  // Initialize SSH session

    // Perform SSH handshake
    int sock = tcpSocket->socketDescriptor();
    int rc = libssh2_session_handshake(session, sock);
    if (rc) {
        QMessageBox::warning(this, "SSH Handshake Failed", "Error code: " + QString::number(rc));
        return;
    }

    // Authenticate using the provided user credentials
    rc = libssh2_userauth_password(session, user.toUtf8().data(), password.toUtf8().data());
    if (rc) {
        QMessageBox::warning(this, "Authentication Failed", "Error code: " + QString::number(rc));
        return;
    }

    // Initialize SFTP session
    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session) {
        rc = libssh2_session_last_errno(session);  // Get the last error from the session
        QMessageBox::warning(this, "SFTP Initialization Failed", "Error code: " + QString::number(rc));
        return;
    }

    // Start by listing the home directory on the remote server
    QString homeDir = "/home/" + user;
    listRemoteFiles(homeDir);

    // List files in the local home directory
    listLocalFiles(QDir::homePath());
}

// Helper function to list files in the remote directory via SFTP
void MainWindow::listRemoteFiles(const QString &remotePath)
{
    ui->remoteFileTree->clear();  // Clear the current file tree

    // Open the remote directory
    LIBSSH2_SFTP_HANDLE *dirHandle = libssh2_sftp_opendir(sftp_session, remotePath.toUtf8().data());
    if (!dirHandle) {
        int err = libssh2_sftp_last_error(sftp_session);  // Get the last SFTP error
        QMessageBox::warning(this, "Error", "Unable to open remote directory: " + remotePath + "\nError code: " + QString::number(err));
        return;
    }

    struct RemoteFileEntry {
        QString fileName;
        QString fullPath;
        bool isDir;
    };

    // List to store files and directories
    QList<RemoteFileEntry> entries;
    char buffer[512];
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    // Read the contents of the directory
    while (libssh2_sftp_readdir(dirHandle, buffer, sizeof(buffer), &attrs) > 0) {
        QString fileName = QString::fromUtf8(buffer);
        QString fullPath = remotePath + "/" + fileName;

        bool isDir = LIBSSH2_SFTP_S_ISDIR(attrs.permissions);

        // Store each entry (file or directory)
        entries.append({fileName, fullPath, isDir});
    }

    libssh2_sftp_closedir(dirHandle);  // Close the directory handle

    // Sort the entries: directories first, then files, all alphabetically
    std::sort(entries.begin(), entries.end(), [](const RemoteFileEntry &a, const RemoteFileEntry &b) {
        if (a.isDir != b.isDir) {
            return a.isDir > b.isDir;  // Directories before files
        }
        return a.fileName.toLower() < b.fileName.toLower();  // Alphabetical order
    });


    // Add sorted items to the UI
    for (const auto &entry : entries) {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->remoteFileTree);
        item->setText(0, entry.fileName);
        item->setData(0, Qt::UserRole, entry.fullPath);
        item->setData(0, Qt::UserRole + 1, entry.isDir);

        if (entry.isDir) {
            item->setText(0, entry.fileName);  // No special [DIR] prefix
        }
    }
}

// Helper function to list files in the local directory
void MainWindow::listLocalFiles(const QString &localPath)
{
    ui->localFileTree->clear();  // Clear the current file tree

    QDir dir(localPath);  // Get the directory object

    QFileInfoList fileList = dir.entryInfoList(QDir::QDir::AllEntries | QDir::Hidden);

    for (const QFileInfo &fileInfo : fileList) {
        QString fileName = fileInfo.fileName();
        QString fullPath = fileInfo.filePath();

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->localFileTree);
        item->setText(0, fileName);
        item->setData(0, Qt::UserRole, fullPath);

        if (fileInfo.isDir()) {
            item->setData(0, Qt::UserRole + 1, true);  // Mark as directory
        } else {
            item->setData(0, Qt::UserRole + 1, false);  // Mark as file
        }

        ui->localFileTree->addTopLevelItem(item);  // Add item to the tree
    }
}

// Function to handle file upload
void MainWindow::uploadFile(const QString &localPath, const QString &remotePath)
{
    // Create a new thread and worker for the file upload
    QThread *thread = new QThread;
    FileTransferWorker *worker = new FileTransferWorker(sftp_session);
    worker->setUploadParams(localPath, remotePath);

    // Connect signals and slots for progress updates and error handling
    connect(thread, &QThread::started, worker, &FileTransferWorker::uploadFile);
    connect(worker, &FileTransferWorker::transferCompleted, this, [this, thread]() {
        QMessageBox::information(this, "Upload", "File uploaded successfully!");
        thread->quit();
    });
    connect(worker, &FileTransferWorker::errorOccurred, this, [this, thread](const QString &error) {
        QMessageBox::warning(this, "Error", error);
        thread->quit();
    });

    // Connect the progressUpdated signal to the progress bar
    connect(worker, &FileTransferWorker::progressUpdated, this, [this](qint64 bytesTransferred, qint64 totalBytes) {
        ui->uploadProgressBar->setMaximum(totalBytes);  // Set the progress bar maximum
        ui->uploadProgressBar->setValue(bytesTransferred);  // Update the progress bar value
    });

    // Cleanup when the thread finishes
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();  // Start the thread
}

// Function to handle file download
void MainWindow::downloadFile(const QString &remotePath, const QString &localPath)
{
    // Create a new thread and worker for the file download
    QThread *thread = new QThread;
    FileTransferWorker *worker = new FileTransferWorker(sftp_session);
    worker->setDownloadParams(remotePath, localPath);

    // Connect signals and slots for progress updates and error handling
    connect(thread, &QThread::started, worker, &FileTransferWorker::downloadFile);
    connect(worker, &FileTransferWorker::transferCompleted, this, [this, thread]() {
        QMessageBox::information(this, "Download", "File downloaded successfully!");
        thread->quit();
    });
    connect(worker, &FileTransferWorker::errorOccurred, this, [this, thread](const QString &error) {
        QMessageBox::warning(this, "Error", error);
        thread->quit();
    });
    connect(worker, &FileTransferWorker::progressUpdated, this, [this](qint64 bytesTransferred, qint64 totalBytes) {
        ui->downloadProgressBar->setMaximum(totalBytes);
        ui->downloadProgressBar->setValue(bytesTransferred);
    });
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void MainWindow::on_remoteTreeContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = ui->remoteFileTree->itemAt(pos);
    if (!item) return;  // If no item was clicked, do nothing

    QString path = item->data(0, Qt::UserRole).toString();  // This is the remote path
    bool isDirectory = item->data(0, Qt::UserRole + 1).toBool();

    QMenu contextMenu;
    QAction *downloadAction = contextMenu.addAction(isDirectory ? "Download Directory as Zip" : "Download File");

    connect(downloadAction, &QAction::triggered, this, [this, path, isDirectory]() {
        if (isDirectory) {
            // Ask the user where to save the zip file locally (on the local machine)
            QString localPath = QFileDialog::getExistingDirectory(this, "Select local directory to save zip");
            if (!localPath.isEmpty()) {
                // Zip the remote directory and download the zip file to the chosen local path
                downloadZippedDirectory(path, localPath);
            }
        } else {
            QString downloadPath = QFileDialog::getSaveFileName(this, "Save file as");
            if (!downloadPath.isEmpty()) {
                // Download a single file using the existing downloadFile function
                downloadFile(path, downloadPath);
            }
        }
    });

    contextMenu.exec(ui->remoteFileTree->viewport()->mapToGlobal(pos));
}

// Function to zip a directory on the remote server
bool MainWindow::zipRemoteDirectory(const QString &remoteDirPath, const QString &remoteZipPath)
{
    // Open an SSH channel to execute the zip command
    LIBSSH2_CHANNEL *channel = libssh2_channel_open_session(session);
    if (!channel) {
        QMessageBox::warning(this, "Error", "Failed to open SSH channel.");
        return false;
    }

    // The zip command to compress the directory into a .zip file on the remote server
    QString command = "zip -r " + remoteZipPath + " " + remoteDirPath;

    int rc = libssh2_channel_exec(channel, command.toUtf8().data());
    if (rc != 0) {
        QMessageBox::warning(this, "Error", "Failed to execute zip command on the server.");
        libssh2_channel_close(channel);
        libssh2_channel_free(channel);
        return false;
    }

    // Wait for the command to complete
    libssh2_channel_wait_closed(channel);
    libssh2_channel_free(channel);

    return true;  // Return success if the directory was zipped successfully
}

void MainWindow::downloadZippedDirectory(const QString &remoteDirPath, const QString &localDirPath)
{
    // Define the remote zip path (e.g., remoteDirPath.zip) on the remote server
    QString remoteZipPath = remoteDirPath + ".zip";

    // Zip the directory on the remote server
    if (!zipRemoteDirectory(remoteDirPath, remoteZipPath)) {
        return;  // Zip failed, return early
    }

    // Choose where to save the local zip file (on your local machine)
    QString localZipPath = QFileDialog::getSaveFileName(this, "Save Zip As", localDirPath + ".zip");
    if (localZipPath.isEmpty()) return;  // User canceled, return early

    // Now, download the remote zip file using the correct remote path
    downloadFile(remoteZipPath, localZipPath);
}
