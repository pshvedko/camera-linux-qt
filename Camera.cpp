//
// Created by shved@mail.ru on 27.07.2020.
//

#include <QSystemTrayIcon>
#include <QMenu>
#include <QHostAddress>
#include <QJsonArray>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>

#include "Camera.h"
#include "QJsonWebToken.h"

#define SETTINGS_WINDOW_TOP "window/top"
#define SETTINGS_WINDOW_LEFT "window/left"
#define SETTINGS_WINDOW_WIDTH "window/width"

Camera::Camera(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    const auto ico = QIcon(":/ico.png");
    setWindowIcon(ico);
    mTray = new QSystemTrayIcon(this);
    mTray->setIcon(ico);
    auto *menu = new QMenu(this);
    auto *show = new QAction(tr("Развернуть окно"), this);
    auto *hide = new QAction(tr("Свернуть окно"), this);
    auto *quit = new QAction(tr("Выход"), this);
    hide->setDisabled(true);
    menu->addAction(show);
    menu->addAction(hide);
    menu->addAction(quit);
    connect(show, &QAction::triggered, this, &Camera::show);
    connect(hide, &QAction::triggered, this, &Camera::hide);
    connect(quit, &QAction::triggered, this, &Camera::quit);
    mHide = hide;
    mShow = show;
    mTray->setContextMenu(menu);
    mTray->show();
    connect(mTray, &QSystemTrayIcon::activated, this, &Camera::toggle);
    mCamera = new QCamera(QCameraInfo::defaultCamera());
    connect(mCamera, &QCamera::stateChanged, this, &Camera::onCameraState);
    mListener = new QTcpServer(this);
    mListener->listen(QHostAddress::Any, 12345);
    connect(mListener, &QTcpServer::newConnection, this, &Camera::onNewConnection);
    QDir configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    mSettings = new QSettings(configDir.filePath("camera.conf"), QSettings::NativeFormat);
    int x = mSettings->value(SETTINGS_WINDOW_TOP, 0).toInt();
    int y = mSettings->value(SETTINGS_WINDOW_LEFT, 0).toInt();
    mWidth = mSettings->value(SETTINGS_WINDOW_WIDTH, 640).toInt();
    mRect.setTopLeft(QPoint(y, x));
    onDisconnected();
}

Camera::~Camera() {
    mSettings->setValue(SETTINGS_WINDOW_TOP, mRect.top());
    mSettings->setValue(SETTINGS_WINDOW_LEFT, mRect.left());
    mSettings->setValue(SETTINGS_WINDOW_WIDTH, mWidth);
    delete mSettings;
    delete mListener;
    delete mCamera;
}

void Camera::toggle(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::MiddleClick:
            if (isHidden())
                show();
            else
                hide();
            break;
        case QSystemTrayIcon::Unknown:
        case QSystemTrayIcon::Context:
        case QSystemTrayIcon::DoubleClick:
            break;
    }
}

void Camera::mouseDoubleClickEvent(QMouseEvent *event) {
    QWidget::mouseDoubleClickEvent(event);
    if (mStream)
        return;
    onDisconnected();
}

void Camera::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent(event);
    if (isVisible())
        event->ignore();
    hide();
}

void Camera::quit() {
    hide();
    close();
}

void Camera::showEvent(QShowEvent *event) {
    if (!mListener->isListening())
        exit(1);
    QWidget::showEvent(event);
    mHide->setEnabled(true);
    mShow->setEnabled(false);
    setGeometry(mRect);
    setWindowFlag(Qt::WindowStaysOnTopHint, true);
    if (mConnected)
        mCamera->start();
}

void Camera::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    mHide->setEnabled(false);
    mShow->setEnabled(true);
    mRect = geometry();
    if (mConnected)
        mCamera->unload();
}

void Camera::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    mRect.setSize(size());
}

void Camera::onNewConnection() {
    QTcpSocket *socket = mListener->nextPendingConnection();
    if (mStream) {
        socket->close();
        return;
    }
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(socket, &QTcpSocket::disconnected, this, &Camera::onDisconnected);
    mStream = new Stream(socket, this);
    connect(mStream, &Stream::connected, this, &Camera::onConnected);
    connect(mStream, &Stream::changed, this, &Camera::onChanged);
    mStream->start();
    connect(socket, &QTcpSocket::readyRead, mStream, &Stream::onReadyRead);
}

static inline int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

void Camera::onConnected(int w, int h) {
    mConnected = true;
    mSurface->setCode();
    mCamera->setViewfinder(mSurface);
    if (!isHidden())
        mCamera->start();
    setSize(w, h);
}

void Camera::setSize(int w, int h) {
    int d = gcd(w, h);
    int p = w / d;
    int q = mWidth / p;
    setFixedSize(q * p, q * h / d);
}

void Camera::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
}

void Camera::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyReleaseEvent(event);
}

void Camera::wheelEvent(QWheelEvent *event) {
    QWidget::wheelEvent(event);
    if (!mConnected)
        return;
    if (event->delta() < 0)
        if (mWidth > 180)
            mWidth -= 30;
        else
            return;
    else if (mWidth < 720)
        mWidth += 30;
    else
        return;
    setSize(width(), height());
}

void Camera::onChanged() {
    mConnected = false;
    if (!isHidden())
        mCamera->unload();
}

void Camera::onDisconnected() {
    delete mStream;
    mStream = nullptr;
    mConnected = false;
    if (!mListener->isListening())
        return;
    QJsonArray hosts;
    for (const auto &a : QNetworkInterface::allAddresses())
        if (a.isGlobal() && a.protocol() == QAbstractSocket::IPv4Protocol)
            hosts.append(a.toString());
    QJsonWebToken token;
    token.appendClaim("host", hosts);
    token.appendClaim("port", mListener->serverPort());
    token.setRandomSecret();
    mSecret = token.getSecret();
    mSurface->setCode(Qr::QrCode::encodeText(token.getToken().toUtf8(), Qr::QrCode::Ecc::LOW));
    int w = 16 + 8 * mSurface->getSize();
    setFixedSize(w, w);
    if (isHidden())
        show();
    else
        activateWindow();
    if (mCamera->state() == QCamera::UnloadedState)
        return;
    mCamera->unload();
    mCamera->setViewfinder(mSurfaceNull);
}

void Camera::onCameraState(QCamera::State state) {
    qDebug() << __PRETTY_FUNCTION__ << state;
    switch (state) {
        case QCamera::LoadedState:
        case QCamera::ActiveState:
            break;
        case QCamera::UnloadedState:
            if (mConnected && !isHidden())
                mCamera->start();
            break;
    }
}

