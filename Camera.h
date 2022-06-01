//
// Created by shved@mail.ru on 27.07.2020.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QCameraInfo>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QTcpServer>
#include <QSettings>
#include <QAudioDeviceInfo>

#include "ui_main.h"
#include "Stream.h"
#include "QAudioInfo.h"

class Camera : public QMainWindow, QCameraInfo, QAudioDeviceInfo, QAudioInfo, Ui::MainWindow {
Q_OBJECT

public:

    explicit Camera(QWidget *parent = nullptr);

    ~Camera() override;

protected:

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

public slots:

    void onCameraState(QCamera::State state);

    void onConnected(int ip, int h);

    void onChanged();

    void onNewConnection();

    void onDisconnected();

    void onVoice(const QByteArray &);

private slots:

    void toggle(QSystemTrayIcon::ActivationReason reason);

    void quit();

    void drop();

signals:

private:

    void setSize(int w, int h);

    QIcon mIco1;
    QIcon mIco2;

    QSystemTrayIcon *mTray;
    QCamera *mCamera;
    QAction *mHide;
    QAction *mShow;
    QAction *mDrop;

    QVideoWidget *mDiscard{};
    QRect mRect;
    QString mSecret;
    QTcpServer *mListener;

    bool mConnected{};

    Stream *mStream{};
    QSettings *mSettings{};

    int mWidth;
};

#endif // MAIN_WINDOW_H
