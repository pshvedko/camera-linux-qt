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

class Queue : public QMutex, QWaitCondition, QQueue<Frame *> {
public:

    virtual ~Queue();

    Frame *pop();

    Frame *get();

    void add(Frame *);

    int count();

    void stash(int);

protected:

    void done();

};


#endif //CAMERA_FRAME_QUEUE_H
