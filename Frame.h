//
// Created by shved@mail.ru on 10.08.2020.
//

#ifndef CAMERA_FRAME_H
#define CAMERA_FRAME_H


#include <QDataStream>

class Queue;

class Frame {

public:

    enum {
        Video = 0x0,
        Audio = 0x1,
        Control = 0x2,
    };

    class Refer {
    public:

        explicit Refer(Frame *frame = nullptr);

        Refer(const Refer &);

        ~Refer();

        void unref() const;

        Frame *operator->() const;

        Frame const &operator*() const;

    private:
        mutable Frame *mFrame;
    };

    class Re {
    public:
        explicit Re(Frame *frame);

        ~Re();

    private:
        Frame *mFrame;
    };

    explicit Frame(Queue *pool);

    virtual ~Frame();

    void recycle();

    void to(Queue *pool);

    friend QDataStream &operator<<(QDataStream &stream, const Frame &frame);

    class Bytes {
    public:

        Bytes &operator()(int size);

        ~Bytes();

        friend QDataStream &operator>>(QDataStream &stream, Bytes &bytes);

        friend QDataStream &operator<<(QDataStream &stream, const Bytes &bytes);

        unsigned char *mData{};
        int mLength{};
    private:
        int mCapacity{};
    };

    Bytes mBytes{};

    short mSize{};
    unsigned char mType{};
    long long mId{};
    int mFlag{};
    int mWidth{};
    int mHeight{};
    int mLength{};

private:

    Queue *mPool{};

};


#endif //CAMERA_FRAME_H
