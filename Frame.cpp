//
// Created by shved@mail.ru on 10.08.2020.
//

#include <QDebug>

#include "Frame.h"

Frame::Frame(Frame::Pool *pool) : mPool(pool) {}

Frame::~Frame() = default;

void Frame::recycle() {
    mPool->add(this);
}

Frame::Bytes &Frame::Bytes::operator()(int size) {
    if (mCapacity < size) {
        mCapacity = size;
        delete[] mData;
        mData = new unsigned char[size];
    }
    mLength = size;
    return *this;
}

Frame::Bytes::~Bytes() {
    delete[] mData;
}

QDataStream &operator>>(QDataStream &stream, Frame::Bytes &bytes) {
    stream.readRawData(reinterpret_cast<char *>(bytes.mData), bytes.mLength);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const Frame::Bytes &bytes) {
    stream.writeRawData(reinterpret_cast<const char *>(bytes.mData), bytes.mLength);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const Frame &frame) {
    switch (frame.mType) {
        case Frame::Audio:
            stream << frame.mType;
            stream << frame.mId;
            stream << frame.mBytes.mLength;
            stream << frame.mBytes;
            break;
    }
    return stream;
}

void Frame::to(Frame::Pool *pool, int limit) {
    if (pool && pool->count() < limit)
        pool->add(this);
    else
        recycle();
}

Frame::Re::Re(Frame *frame) {
    mFrame = frame;
}

Frame::Re::~Re() {
    mFrame->recycle();
}

Frame::Refer::Refer(Frame *frame) : mFrame(frame) {}

Frame::Refer::Refer(const Frame::Refer &copy) {
    mFrame = copy.mFrame;
    copy.unref();
}

Frame::Refer::~Refer() {
    delete mFrame;
}

void Frame::Refer::unref() const {
    mFrame = nullptr;
}

Frame *Frame::Refer::operator->() const {
    return mFrame;
}

Frame const &Frame::Refer::operator*() const {
    return *mFrame;
}

