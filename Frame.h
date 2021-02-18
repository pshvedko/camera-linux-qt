//
// Created by shved@mail.ru on 10.08.2020.
//

#ifndef CAMERA_FRAME_H
#define CAMERA_FRAME_H


#include <QDataStream>

class Frame {

public:

    enum {
        Video = 0x0,
        Audio = 0x1,
        Control = 0x2,
    };

    class Pool {
    public:
        virtual void add(Frame *frame) = 0;

        virtual int count() = 0;
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

    explicit Frame(Pool *pool);

    virtual ~Frame();

    void recycle();

    void to(Pool *pool);

    Pool * pool() const;

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

    Pool *mLoop{};

private:

    Pool *mPool{};

};


#endif //CAMERA_FRAME_H
