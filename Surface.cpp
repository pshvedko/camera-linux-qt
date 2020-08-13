//
// Created by shved@mail.ru on 28.07.2020.
//

#include <QDebug>
#include <QPainter>

#include "Surface.h"

void Surface::paintEvent(QPaintEvent *event) {
    QVideoWidget::paintEvent(event);
    if (!mCode)
        return;
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    int size = mCode->getSize();
    const double scale = 1. * std::min(width(), height()) / (size-- + 2);
    const double half = scale / 2;
    for (int y = 0; y <= size; y++) {
        for (int x = 0; x <= size; x++) {
            if (mCode->getModule(x, y)) {
                painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, false);
                bool l = x && mCode->getModule(x - 1, y);
                bool r = x < size && mCode->getModule(x + 1, y);
                bool t = y && mCode->getModule(x, y - 1);
                bool b = y < size && mCode->getModule(x, y + 1);
                if (l)
                    painter.drawRect(QRectF(scale + x * scale, scale + y * scale, half, scale));
                if (r)
                    painter.drawRect(QRectF(scale + x * scale + half, scale + y * scale, half, scale));
                if (t)
                    painter.drawRect(QRectF(scale + x * scale, scale + y * scale, scale, half));
                if (b)
                    painter.drawRect(QRectF(scale + x * scale, scale + y * scale + half, scale, half));
                if (l && r)
                    continue;
                if (t && b)
                    continue;
                painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
                painter.drawEllipse(QRectF(scale + x * scale, scale + y * scale, scale, scale));
            }
        }
    }
}

void Surface::setCode(const Qr::QrCode &code) {
    setCode(new Qr::QrCode(code));
}

void Surface::setCode(Qr::QrCode *code) {
    delete mCode;
    QPalette paint = palette();
    paint.setColor(QPalette::Background, code ? Qt::transparent : Qt::black);
    setPalette(paint);
    mCode = code;
    repaint();
}

int Surface::getSize() {
    if (mCode)
        return mCode->getSize();
    return 0;
}

Surface::~Surface() {
    delete mCode;
}

Surface::Surface(QWidget *parent) : QCameraViewfinder(parent) {
    setAutoFillBackground(true);
}
