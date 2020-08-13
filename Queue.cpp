//
// Created by shved@mail.ru on 10.08.2020.
//

#include <QDebug>
#include <QMutexLocker>

#include "Queue.h"

Frame *Queue::pop() {
    QMutexLocker locker(this);
    if (empty())
        if (wait(this))
            if (empty())
                return new Frame(this);
    return dequeue();
}

Frame *Queue::get() {
    QMutexLocker locker(this);
    if (empty())
        return new Frame(this);
    return dequeue();
}

void Queue::add(Frame *frame) {
    QMutexLocker locker(this);
    enqueue(frame);
    wakeOne();
}

void Queue::done() {
    add(nullptr);
}

Queue::~Queue() {
    QMutexLocker locker(this);
    while (!empty()) delete dequeue();
}

int Queue::count() {
    QMutexLocker locker(this);
    return size();
}
