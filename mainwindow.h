#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTcpSocket>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <QSettings>
#include <QComboBox>
#include <QCheckBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "ui_mainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QGraphicsScene *scene;  // Graphics scene to hold the image
    QGraphicsView *graphicsView;

private slots:
    void on_connectButton_clicked();
    void on_itemClicked(QTreeWidgetItem *item);
    void on_localItemClicked(QTreeWidgetItem *item);

    // For auto-filling based on history
    void updatePasswordField();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    // Helper methods
    void connectToServer(const QString &host, const QString &user, const QString &password);
    void listRemoteFiles(const QString &remotePath);
    void listLocalFiles(const QString &localPath);
    void downloadFile(const QString &remotePath, const QString &localPath);
    void uploadFile(const QString &localPath, const QString &remotePath);
    void downloadZippedDirectory(const QString &remoteDirPath, const QString &localDirPath);
    // Methods for managing user input history
    void saveLoginDetails(const QString &host, const QString &user, const QString &password);
    void loadLoginDetails();
    bool zipRemoteDirectory(const QString &remoteDirPath, const QString &remoteZipPath);
    void on_remoteTreeContextMenuRequested(const QPoint &pos);
    // Auto-fill history from QSettings
    QSettings settings;
};

#endif // MAINWINDOW_H
