//
// Created by shved on 10.08.2020.
//

#ifndef CAMERA_FRAME_QUEUE_H
#define CAMERA_FRAME_QUEUE_H


#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

#include "Frame.h"

class Queue : public Frame::Pool, QMutex, QWaitCondition, QQueue<Frame *> {
public:

    virtual ~Queue();

    Frame *pop();

    Frame *get();

    void add(Frame *) override;

    int count() override;

protected:

    void done();

};


#endif //CAMERA_FRAME_QUEUE_H
