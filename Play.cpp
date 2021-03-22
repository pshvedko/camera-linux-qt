//
// Created by shved@mail.ru on 12.08.2020.
//
// aplay -D hw:2,1 out.wav

#include <QDebug>

#include "Play.h"

Play::Play(const char *device, uint32_t channels, QObject *parent) :
        Loop(parent), Audio(device, 44100, channels, SND_PCM_STREAM_PLAYBACK),
        mChannels(channels), mBuffSize(16), mSkip(false) {
    int err;
    mCodec = opus_decoder_create(48000, channels, &err);
    if (err != OPUS_OK) {
        qDebug() << "opus_decoder_create" << opus_strerror(err);
        return;
    }
}

void Play::loop(Frame *frame, Queue *queue) {
    Frame::Re re(frame);
    if (!mCodec)
        return;
    if (frame->mType != Frame::Audio)
        return;
    signed short buffer[8192 * mChannels];
    int size = opus_decode(mCodec, frame->mBytes.mData, frame->mBytes.mLength, buffer, 8192, 0);
    if (size > 0) {
        auto n = queue->count();
        if (mSkip) {
            if (n > mBuffSize)
                return;
            mSkip = false;
            qDebug() << __PRETTY_FUNCTION__ << "Play" << n;
        } else if (n > mBuffSize * 2) {
            mSkip = true;
            qDebug() << __PRETTY_FUNCTION__ << "Drop" << n;
        } else if (n < mBuffSize / 2) {
            queue->stash(mBuffSize);
            qDebug() << __PRETTY_FUNCTION__ << "Buffering" << n;
        }
        emit voice(QByteArray(256, 0));
        play(buffer, size);
    }
}

Play::~Play() {
    done();
    quit();
    wait();
    opus_decoder_destroy(mCodec);
}

