//
// Created by shved@mail.ru on 08.08.2020.
//

#ifndef CAMERA_CAPTURE_H
#define CAMERA_CAPTURE_H

#include <opus/opus.h>

#include "Loop.h"
#include "Audio.h"

class Capture : public Loop, Audio {
Q_OBJECT

public :

    Capture(const char device[], int samples, int channels, QObject *parent);

    ~Capture() override ;

signals:

    void write(Frame::Refer frame);

protected:

    void loop(Frame *frame) override;

private:

    OpusEncoder *mCodec;
    int mChannels;
    int mSamples;
};


#endif //CAMERA_CAPTURE_H
