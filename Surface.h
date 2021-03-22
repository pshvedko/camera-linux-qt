//
// Created by shved@mail.ru on 28.07.2020.
//

#ifndef CAMERA_VIEW_FINDER_H
#define CAMERA_VIEW_FINDER_H

#include <QCameraViewfinder>
#include "QrCode.h"


class Surface : public QCameraViewfinder {
Q_OBJECT


public:
    explicit Surface(QWidget *parent = nullptr);

    ~Surface() override;

    void setCode(const Qr::QrCode &code);

    void setCode(Qr::QrCode *code = nullptr);

    int getSize();

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:

    void onVoice(const QByteArray&);

private slots:

private:
    Qr::QrCode *mCode{};
};


#endif //CAMERA_VIEW_FINDER_H
