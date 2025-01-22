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
#include <QFile>
#include <libssh2.h>
#include <QEvent>
#include <QObject>
#include <QMenu>

// Constructor of MainWindow class
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), tcpSocket(new QTcpSocket(this)), session(nullptr), sftp_session(nullptr), settings("Sativa.inc", "SSHFiletransfer")
{
    ui->setupUi(this);
    this->installEventFilter(this);

    ui->connectButton->setDefault(true);

    // Create and set the QGraphicsScene
    // QGraphicsScene *scene = new QGraphicsScene(this);
    // ui->graphicsView->setScene(scene);

    // Debugging output
    // qDebug() << "graphicsView initialized and scene set.";

    // Load an image from resources (check the file path in the .qrc file)
    // QPixmap pixmap(":/resources/cat.png");
    // if (pixmap.isNull()) {
    //    qDebug() << "Failed to load image ':/resources/cat.png'. Ensure it's in the resources.";
    // } else {
    //   QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    //    scene->addItem(item);
    //    qDebug() << "Image successfully loaded and added to the scene.";
    //}

    // Initialize UI components and connections as needed
    ui->uploadProgressBar->setValue(0);
    ui->downloadProgressBar->setValue(0);

    ui->remoteFileTree->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->ipComboBox->setEditable(true);
    ui->ipComboBox->lineEdit()->setPlaceholderText("Host Address");

    ui->usernameComboBox->setEditable(true);
    ui->usernameComboBox->lineEdit()->setPlaceholderText("Username");


    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Load login history on application start
    loadLoginDetails();

    // Setup context menu, signal connections, etc.
    connect(ui->remoteFileTree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::on_remoteTreeContextMenuRequested);
    connect(ui->ipComboBox, &QComboBox::editTextChanged, this, &MainWindow::updatePasswordField);
    connect(ui->usernameComboBox, &QComboBox::editTextChanged, this, &MainWindow::updatePasswordField);
    connect(ui->remoteFileTree, &QTreeWidget::itemClicked, this, &MainWindow::on_itemClicked);
    connect(ui->localFileTree, &QTreeWidget::itemClicked, this, &MainWindow::on_localItemClicked);

    connect(ui->checkBox, &QCheckBox::checkStateChanged, this, [this]() {
        listRemoteFiles("/home/" + ui->usernameComboBox->currentText());
        listLocalFiles(QDir::homePath());
    });

    ui->connectButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgb(0, 12, 177);"  // Default color
        "   color: white;"               // Text color
        "   border: none;"
        "   padding: 5px 10px;"
        "   font-size: 16px;"
        "   border-radius: 5px;"
        "   transition: all 0.3s ease;"  // Smooth transition
        "}"
        "QPushButton:hover {"
        "   background-color: #070056;"  // Hover color
        "}"
        "QPushButton:pressed {"
        "   background-color: #2a63ff;"  // Pressed color
        "   padding: 9px 19px;"          // Slightly smaller size
        "}"
        );

    ui->savePasswordCheckBox->setStyleSheet(
        "QCheckBox::indicator {"
        "   width: 16px; height: 16px;"  // Explicit size
        "   border: 1px solid black;"   // Visible border
        "   background-color: white;"  // Visible background
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: blue;"
        "}"
        );

    ui->checkBox->setStyleSheet(
        "QCheckBox::indicator {"
        "   width: 16px; height: 16px;"  // Explicit size
        "   border: 1px solid black;"   // Visible border
        "   background-color: white;"  // Visible background
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: blue;"
        "}"
        );


    QHeaderView *header = ui->remoteFileTree->header();
    header->setStyleSheet("QHeaderView::section {"
                          "background-color: rgb(0, 12, 177);"  // Blue background
                          "color: rgb(255, 255, 255);"               // White text
                          "font-weight: bold;"           // Bold text
                          "font-size: 16px;"
                          "text-align: center;"         // Center text
                          "border: 1px solid #005499;"  // Slight border
                          "}");

    // Apply the same for localFileTree
    ui->localFileTree->header()->setStyleSheet(header->styleSheet());
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            // Trigger Connect button's action
            ui->connectButton->click();
            return true; // Mark the event as handled
        }
    }

    // Pass the event to the base class for default handling
    return QMainWindow::eventFilter(watched, event);

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

void MainWindow::on_itemClicked(QTreeWidgetItem *item) {
    QString path = item->data(0, Qt::UserRole).toString();
    bool isDirectory = item->data(0, Qt::UserRole + 1).toBool();

    if (isDirectory) {
        listRemoteFiles(path); // Navigate to the clicked directory
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

    // Update the password only if the "Save Password" checkbox is checked
    if (ui->savePasswordCheckBox->isChecked() && !password.isEmpty()) {
        settings.setValue("password", password);
    }

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
    // Clear existing items to avoid duplicates
    ui->ipComboBox->clear();
    ui->usernameComboBox->clear();

    // Load IP history and fill the combobox
    QStringList ipHistory = settings.value("ipHistory").toStringList();
    ui->ipComboBox->addItems(ipHistory);

    // Load username history and fill the combobox
    QStringList userHistory = settings.value("userHistory").toStringList();
    ui->usernameComboBox->addItems(userHistory);

    // Ensure fields start empty
    ui->ipComboBox->setCurrentIndex(-1);
    ui->usernameComboBox->setCurrentIndex(-1);
    ui->passwordLineEdit->clear();
}

void MainWindow::updatePasswordField()
{
    QString host = ui->ipComboBox->currentText();
    QString user = ui->usernameComboBox->currentText();

    // Retrieve the password based on the host and username
    settings.beginGroup(host + "/" + user);
    QString savedPassword = settings.value("password", "").toString();
    settings.endGroup();

    if (!savedPassword.isEmpty()) {
        // Auto-fill the password if it exists
        ui->passwordLineEdit->setText(savedPassword);
    } else {
        // Clear the password field if no saved password
        ui->passwordLineEdit->clear();
    }
}

void MainWindow::connectToServer(const QString &host, const QString &user, const QString &password) {

    // Connect to SSH server via TCP
    tcpSocket->connectToHost(host, 22); // Connect to SSH port 22
    if (!tcpSocket->waitForConnected(5000)) { // Wait up to 5 seconds for connection
        QMessageBox::warning(this, "Connection Failed", "Unable to connect to SSH server.");
        return;
    }
    // Initialize libssh2
    if (libssh2_init(0)) {
        QMessageBox::critical(this, "Error", "Failed to initialize libssh2.");
        return;
    }
    // Create the libssh2 session
    session = libssh2_session_init();
    if (!session) {
        QMessageBox::critical(this, "Error", "Failed to initialize SSH session.");
        libssh2_exit();
        return;
    }

    // Get the socket descriptor
    int sock = tcpSocket->socketDescriptor();
    if (sock <= 0) {
        QMessageBox::critical(this, "Socket Error", "Invalid socket descriptor.");
        libssh2_session_free(session);
        libssh2_exit();
        return;
    }

    // Log supported algorithms for debugging
    qDebug() << "Supported KEX algorithms: " << libssh2_session_methods(session, LIBSSH2_METHOD_KEX);
    qDebug() << "Supported HostKey algorithms: " << libssh2_session_methods(session, LIBSSH2_METHOD_HOSTKEY);
    qDebug() << "Supported Crypt (CS): " << libssh2_session_methods(session, LIBSSH2_METHOD_CRYPT_CS);
    qDebug() << "Supported Crypt (SC): " << libssh2_session_methods(session, LIBSSH2_METHOD_CRYPT_SC);

    // Perform the SSH handshake
    int rc = libssh2_session_handshake(session, sock);
    if (rc) {
        char *errmsg;
        libssh2_session_last_error(session, &errmsg, nullptr, 0);
        qDebug() << "Handshake error code: " << rc;
        qDebug() << "libssh2 error message: " << errmsg;

        QMessageBox::critical(this, "SSH Handshake Error", "Unable to perform handshake.");
        libssh2_session_free(session);
        libssh2_exit();
        return;
    }

    // Retrieve the host key from the server
    size_t keyLen;
    int keyType;
    const char *hostKey = libssh2_session_hostkey(session, &keyLen, &keyType);
    if (!hostKey) {
        QMessageBox::warning(this, "Host Key Retrieval Failed", "Unable to retrieve host key.");
        libssh2_session_disconnect(session, "Host key retrieval failure");
        libssh2_session_free(session);
        libssh2_exit();
        return;
    }

    // Convert the host key to a fingerprint for display
    QByteArray fingerprint = QByteArray(hostKey, keyLen).toHex(':');
    QString fingerprintStr = QString("Fingerprint: %1").arg(QString(fingerprint));
    qDebug() << "Retrieved host key fingerprint: " << fingerprintStr;

    // Load known hosts
    LIBSSH2_KNOWNHOSTS *knownHosts = libssh2_knownhost_init(session);
    if (!knownHosts) {
        QMessageBox::critical(this, "Error", "Failed to initialize known hosts.");
        libssh2_session_disconnect(session, "Known hosts initialization failure");
        libssh2_session_free(session);
        libssh2_exit();
        return;
    }

    // Ensure the known_hosts file exists
    QFile knownHostsFile(QDir::homePath() + "/.ssh/known_hosts");

    if (!knownHostsFile.exists()) {
        QDir().mkpath(QDir::homePath() + "/.ssh");
        knownHostsFile.open(QIODevice::WriteOnly); // Create an empty file
        knownHostsFile.close();
    }

    // Read the known_hosts file
    libssh2_knownhost_readfile(knownHosts, knownHostsFile.fileName().toUtf8().data(), LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    qDebug() << "Host key type:" << keyType << ", Key length:" << keyLen;

    // Check the retrieved host key against the known hosts file
    struct libssh2_knownhost *hostEntry;
    int checkResult = libssh2_knownhost_checkp(
        knownHosts,
        host.toUtf8().data(),
        22,
        hostKey,
        keyLen,
        LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW,
        &hostEntry);

    // If the host key is not known, prompt the user to accept or reject
    if (checkResult != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
        QString message = QString(
                              "The authenticity of host '%1' cannot be established.\n"
                              "%2\n"
                              "Do you want to continue connecting?")
                              .arg(host, fingerprintStr);

        QMessageBox::StandardButton response = QMessageBox::question(
            this, "New Host Key", message, QMessageBox::Yes | QMessageBox::No);

        if (response == QMessageBox::No) {
            libssh2_session_disconnect(session, "User declined host key");
            libssh2_session_free(session);
            libssh2_knownhost_free(knownHosts);
            libssh2_exit();
            return;
        }

        qDebug() << "Host key type:" << keyType << ", Key length:" << keyLen;
        // Add the host key to the known_hosts file

        // Determine the correct key type flag
        int keyFlag;
        if (keyType == LIBSSH2_HOSTKEY_TYPE_RSA) {
            keyFlag = LIBSSH2_KNOWNHOST_KEY_SSHRSA;
        } else if (keyType == LIBSSH2_HOSTKEY_TYPE_DSS) {
            keyFlag = LIBSSH2_KNOWNHOST_KEY_SSHDSS;
        } else if (keyType == LIBSSH2_HOSTKEY_TYPE_ECDSA_256) {
            keyFlag = LIBSSH2_KNOWNHOST_KEY_ECDSA_256;
        } else if (keyType == LIBSSH2_HOSTKEY_TYPE_ED25519) {
            keyFlag = LIBSSH2_KNOWNHOST_KEY_ED25519;
        } else {
            qDebug() << "Unsupported host key type:" << keyType;
            QMessageBox::warning(this, "Error", "Unsupported host key type.");
            return;
        }

        // Add the host key to the known hosts list
        int addResult = libssh2_knownhost_add(
            knownHosts,
            host.toUtf8().data(),
            nullptr,  // No resolved address
            hostKey,
            keyLen,
            LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW | keyFlag,
            nullptr);

        if (addResult != 0) {
            qDebug() << "libssh2_knownhost_add failed. Error code:" << addResult;
            QMessageBox::warning(this, "Error", "Failed to add host key to known_hosts. Error code: " + QString::number(addResult));
            return;
        }

        // Save to the known_hosts file
        int writeResult = libssh2_knownhost_writefile(
            knownHosts,
            knownHostsFile.fileName().toUtf8().data(),
            LIBSSH2_KNOWNHOST_FILE_OPENSSH);

        if (writeResult != 0) {
            qDebug() << "libssh2_knownhost_writefile failed. Error code:" << writeResult;
            QMessageBox::warning(this, "Error", "Failed to write to known_hosts file. Error code: " + QString::number(writeResult));
        } else {
            qDebug() << "Successfully added host key to known_hosts.";
        }
    }
    libssh2_knownhost_free(knownHosts);

    // Authenticate user with the provided password
    if (libssh2_userauth_password(session, user.toUtf8().data(), password.toUtf8().data())) {
        QMessageBox::warning(this, "Authentication Failed", "Invalid username or password.");
        libssh2_session_disconnect(session, "Authentication failure");
        libssh2_session_free(session);
        libssh2_exit();
        return;
    }
    qDebug() << "User authentication succeeded.";

    // Initialize the SFTP session
    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session) {
        rc = libssh2_session_last_errno(session);
        QMessageBox::warning(this, "SFTP Initialization Failed", "Error code: " + QString::number(rc));
        libssh2_session_disconnect(session, "SFTP session failure");
        libssh2_session_free(session);
        libssh2_exit();
        return;
    }
    qDebug() << "SFTP session initialized successfully.";

    // List remote and local files
    QString homeDir = "/home/" + user;
    listRemoteFiles(homeDir);
    listLocalFiles(QDir::homePath());
}

void MainWindow::listRemoteFiles(const QString &remotePath) {
    if (!sftp_session) { // Ensure SFTP session is initialized
        QMessageBox::warning(this, "Error", "SFTP session not initialized.");
        return;
    }

    // Open the remote directory
    LIBSSH2_SFTP_HANDLE *dirHandle = libssh2_sftp_opendir(sftp_session, remotePath.toUtf8().data());
    if (!dirHandle) {
        int err = libssh2_sftp_last_error(sftp_session);  // Get the last SFTP error
        QMessageBox::warning(this, "Error", "Unable to open remote directory: " + remotePath + "\nError code: " + QString::number(err));
        return;
    }

    struct FileItem {
        QString name;
        QString fullPath;
        bool isDirectory;
        bool isHidden;
    };

    QList<FileItem> fileList;

    char buffer[512];  // Buffer for file names
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    while (libssh2_sftp_readdir(dirHandle, buffer, sizeof(buffer), &attrs) > 0) {
        QString fileName = QString::fromUtf8(buffer);
        QString fullPath = remotePath + "/" + fileName;

        bool isHidden = fileName.startsWith(".");
        bool isDirectory = LIBSSH2_SFTP_S_ISDIR(attrs.permissions);

        // Exclude the ".." entry from the directory listing
        if (fileName == "..") {
            continue;
        }

        fileList.append({fileName, fullPath, isDirectory, isHidden});
    }

    libssh2_sftp_closedir(dirHandle);

    bool showHidden = ui->checkBox->isChecked();

    // Sort the file list
    std::sort(fileList.begin(), fileList.end(), [](const FileItem &a, const FileItem &b) {
        if (a.isDirectory != b.isDirectory)
            return a.isDirectory; // Directories first
        if (a.isHidden != b.isHidden)
            return !a.isHidden; // Non-hidden files before hidden
        return a.name.compare(b.name, Qt::CaseInsensitive) < 0; // Ascending order
    });

    ui->remoteFileTree->clear();  // Clear the file tree

    // Add "Up Directory" at the top of the list
    QTreeWidgetItem *upItem = new QTreeWidgetItem(ui->remoteFileTree);
    upItem->setText(0, "..");
    upItem->setData(0, Qt::UserRole, remotePath + "/..");
    upItem->setData(0, Qt::UserRole + 1, true); // Treat as a directory

    // Populate the rest of the list
    for (const FileItem &file : fileList) {
        if (!showHidden && file.isHidden) {
            continue;
        }

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->remoteFileTree);
        item->setText(0, file.name);
        item->setData(0, Qt::UserRole, file.fullPath);
        item->setData(0, Qt::UserRole + 1, file.isDirectory);
    }
}

// Helper function to list files in the local directory
void MainWindow::listLocalFiles(const QString &localPath) {
    ui->localFileTree->clear();

    QDir dir(localPath);

    bool showHidden = ui->checkBox->isChecked();

    QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::Hidden);

    struct FileItem {
        QString name;
        QString fullPath;
        bool isDirectory;
        bool isHidden;
    };

    QList<FileItem> sortedFileList;

    for (const QFileInfo &fileInfo : fileList) {
        QString fileName = fileInfo.fileName();
        QString fullPath = fileInfo.filePath();

        bool isHidden = fileName.startsWith(".");
        bool isDirectory = fileInfo.isDir();

        // Exclude the ".." entry from the directory listing
        if (fileName == "..") {
            continue;
        }

        sortedFileList.append({fileName, fullPath, isDirectory, isHidden});
    }

    // Sort the file list
    std::sort(sortedFileList.begin(), sortedFileList.end(), [](const FileItem &a, const FileItem &b) {
        if (a.isDirectory != b.isDirectory)
            return a.isDirectory; // Directories first
        if (a.isHidden != b.isHidden)
            return !a.isHidden; // Non-hidden files before hidden
        return a.name.compare(b.name, Qt::CaseInsensitive) < 0; // Ascending order
    });

    // Add "Up Directory" at the top of the list
    QTreeWidgetItem *upItem = new QTreeWidgetItem(ui->localFileTree);
    upItem->setText(0, "..");
    upItem->setData(0, Qt::UserRole, dir.absolutePath() + "/..");
    upItem->setData(0, Qt::UserRole + 1, true); // Treat as a directory

    for (const FileItem &file : sortedFileList) {
        if (!showHidden && file.isHidden) {
            continue;
        }

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->localFileTree);
        item->setText(0, file.name);
        item->setData(0, Qt::UserRole, file.fullPath);
        item->setData(0, Qt::UserRole + 1, file.isDirectory);
    }
}

// Function to handle file upload
void MainWindow::uploadFile(const QString &localPath, const QString &remotePath)
{
    // Create a new thread and worker for the file upload
    QThread *thread = new QThread;
    FileTransferWorker *worker = new FileTransferWorker(sftp_session, session, this);
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
    FileTransferWorker *worker = new FileTransferWorker(sftp_session, session, this);
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
            QString localDir = QFileDialog::getExistingDirectory(this, "Select destination directory");
            if (!localDir.isEmpty()) {
                // Generate a zip filename based on the directory name
                QString dirName = QFileInfo(path).fileName();
                QString localZipPath = localDir + "/" + dirName + ".zip";

                // Zip the directory and download it
                downloadZippedDirectory(path, localZipPath);
            }
        } else {
            QString downloadPath = QFileDialog::getSaveFileName(this, "Save file as");
            if (!downloadPath.isEmpty()) {
                downloadFile(path, downloadPath);
            }
        }
    });

    contextMenu.exec(ui->remoteFileTree->viewport()->mapToGlobal(pos));
}

// Function to zip a directory on the remote server
bool MainWindow::zipRemoteDirectory(const QString &remoteDirPath, const QString &remoteZipPath) {
    LIBSSH2_CHANNEL *channel = libssh2_channel_open_session(session);
    if (!channel) {
        QMessageBox::warning(this, "Error", "Failed to open SSH channel.");
        return false;
    }

    // Get the parent directory and target directory name
    QString parentDir = QFileInfo(remoteDirPath).absolutePath();
    QString targetDir = QFileInfo(remoteDirPath).fileName();

    // Check if the remoteZipPath already has ".zip"
    QString finalZipPath = remoteZipPath.endsWith(".zip") ? remoteZipPath : remoteZipPath + ".zip";

    // Change to the parent directory and zip the target directory
    QString command = QString("cd \"%1\" && zip -r \"%2\" \"%3\"")
                          .arg(parentDir, finalZipPath, targetDir);
    qDebug() << "Executing command on server:" << command;

    int rc = libssh2_channel_exec(channel, command.toUtf8().data());
    if (rc != 0) {
        QMessageBox::warning(this, "Error", "Failed to execute zip command on the server.");
        libssh2_channel_close(channel);
        libssh2_channel_free(channel);
        return false;
    }

    // Read command output for debugging
    QByteArray output;
    char buffer[1024];
    while (true) {
        ssize_t bytesRead = libssh2_channel_read(channel, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            output.append(buffer, bytesRead);
        } else if (bytesRead == LIBSSH2_ERROR_EAGAIN) {
            continue; // Non-blocking mode, retry
        } else {
            break; // End of output
        }
    }

    qDebug() << "Command output:" << output;

    // Wait for the command to complete
    libssh2_channel_wait_closed(channel);
    libssh2_channel_free(channel);

    return true;
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
    QString localZipPath = QFileDialog::getSaveFileName(this, "Save Zip As", localDirPath);
    if (localZipPath.isEmpty()) return;  // User canceled, return early

    // Now, download the remote zip file using the correct remote path
    downloadFile(remoteZipPath, localZipPath);
}

