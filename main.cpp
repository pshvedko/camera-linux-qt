//
// Created by shved on 27.07.2020.
//

#include <QApplication>

#include "Camera.h"

Q_DECLARE_METATYPE(Frame::Refer)

void quiet(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    switch (type) {
        case QtDebugMsg:
            qt_message_output(type, context, message);
            break;
        case QtWarningMsg:
        case QtCriticalMsg:
        case QtFatalMsg:
        case QtInfoMsg:
            break;
    }
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler(quiet);
    qRegisterMetaType<Frame::Refer>();
    QApplication app(argc, argv);
    Camera camera;
    camera.show();
    return QApplication::exec();
}
