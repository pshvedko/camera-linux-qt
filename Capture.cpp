//
// Created by shved@mail.ru on 08.08.2020.
//
// http://equalarea.com/paul/alsa-audio.html
// arecord -D hw:2,1 -f S16_LE -r 44100 -c 2 | aplay -D hw:0,0
//
// pactl list short sinks
// pactl set-default-sink 0

#include <QDebug>

#include "Capture.h"


Capture::Capture(const char device[], int samples, int channels, QObject *parent) :
        Loop(parent), Audio(device, 44100, channels, SND_PCM_STREAM_CAPTURE),
        mSamples(samples), mChannels(channels) {
    int err;
    mCodec = opus_encoder_create(48000, channels, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        qDebug() << "opus_encoder_create" << opus_strerror(err);
        return;
    }
}

void Capture::loop(Frame *frame) {
    signed short buffer[mSamples * mChannels];
    if (record(buffer, mSamples)) {
        frame->mId = 0;
        frame->mBytes(4096);
        frame->mType = Frame::Audio;
        opus_int32 size = opus_encode(mCodec, buffer, mSamples, frame->mBytes.mData, frame->mBytes.mLength);
        if (size > 2) {
            frame->mBytes(size);
            emit write(Frame::Refer(frame));
            return;
        }
    }
    frame->recycle();
}

Capture::~Capture() {
    done();
    quit();
    wait();
    opus_encoder_destroy(mCodec);
}
