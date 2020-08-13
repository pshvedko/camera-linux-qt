//
// Created by shved on 10.08.2020.
//

#ifndef CAMERA_LOOP_H
#define CAMERA_LOOP_H


#include <QThread>

#include "Queue.h"

class QMutex;

class Loop : public QThread, public Queue {
public:
    explicit Loop(QObject *parent);

    bool wait();

protected:
    void run() override;

    virtual void loop(Frame *frame) = 0;
};


#endif //CAMERA_LOOP_H
