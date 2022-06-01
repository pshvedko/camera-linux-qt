//
// Created by shved@mail.ru on 10.08.2020.
//

#include "Stream.h"

Stream::Stream(QTcpSocket *socket, QObject *parent) :
        Capture(AUDIO_DEVICE, 480, 2, parent),
        mStream(socket), mSocket(socket) {
    connect(this, &Stream::write, this, &Stream::onWriteReady);
    mPlay = new Play(AUDIO_DEVICE, 2, this);
    connect(mPlay, &Play::voice, this, &Stream::onVoice);
    mPlay->start();
}

Stream::~Stream() {
    delete mFrame;
    delete mShow;
    delete mPlay;
}

void Stream::onReadyRead() {
    read();
}

Stream::Reader Stream::readBegin() {
    if (isEnough(2))
        return &Stream::readBegin;
    mFrame = get();
    mStream >> mFrame->mSize;
    return readToken();
}

Stream::Reader Stream::readToken() {
    if (isEnough(mFrame->mSize))
        return &Stream::readToken;
    mStream >> mFrame->mBytes(mFrame->mSize);
    return readType();
}

Stream::Reader Stream::readType() {
    if (isEnough(1))
        return &Stream::readType;
    mStream >> mFrame->mType;
    switch (mFrame->mType) {
        case Frame::Video | Frame::Control:
            return readWidth();
        case Frame::Video:
            return readFlag();
        case Frame::Audio | Frame::Control:
            return readLength();
        case Frame::Audio:
            return readId();
    }
    mSocket->close();
    return nullptr;
}

Stream::Reader Stream::readId() {
    if (isEnough(8))
        return &Stream::readId;
    mStream >> mFrame->mId;
    return readLength();
}

Stream::Reader Stream::readFlag() {
    if (isEnough(4))
        return &Stream::readFlag;
    mStream >> mFrame->mFlag;
    return readId();
}

Stream::Reader Stream::readWidth() {
    if (isEnough(4))
        return &Stream::readWidth;
    mStream >> mFrame->mWidth;
    return readHeight();
}

Stream::Reader Stream::readHeight() {
    if (isEnough(4))
        return &Stream::readHeight;
    mStream >> mFrame->mHeight;
    return readLength();
}

Stream::Reader Stream::readLength() {
    if (isEnough(4))
        return &Stream::readLength;
    mStream >> mFrame->mLength;
    return readPayload();
}

Stream::Reader Stream::readPayload() {
    if (isEnough(mFrame->mLength))
        return &Stream::readPayload;
    mStream >> mFrame->mBytes(mFrame->mLength);
    switch (mFrame->mType) {
        case Frame::Video | Frame::Control:
            emit changed();
            delete mShow;
            mShow = new Show(VIDEO_DEVICE, mFrame->mWidth, mFrame->mHeight, this);
            mShow->start();
            emit connected(mFrame->mWidth, mFrame->mHeight);
            mFrame->recycle();
            break;
        case Frame::Video:
            mFrame->to(mShow);
            break;
        case Frame::Audio | Frame::Control:
            mFrame->recycle();
            break;
        case Frame::Audio:
            mFrame->to(mPlay);
            break;
    }
    mFrame = get();
    return readType();
}

void Stream::onWriteReady(const Frame::Refer &frame) {
    mStream << *frame;
    frame->recycle();
    frame.unref();
}

void Stream::onVoice(const QByteArray &a) {
    emit voice(a);
}

void Stream::close() {
    mSocket->close();
}
