//
// Created by shved@mail.ru on 10.08.2020.
//

#include <QDebug>

#include "Loop.h"

Loop::Loop(QObject *parent) : QThread(parent) {}

void Loop::run() {
    while (true) {
        Frame *frame = pop();
        if (!frame)
            break;
        loop(frame);
    }
}

bool Loop::wait() {
    return QThread::wait();
}
