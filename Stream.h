//
// Created by shved@mail.ru on 10.08.2020.
//

#ifndef CAMERA_STREAM_H
#define CAMERA_STREAM_H

#include <QTcpSocket>
#include <QDataStream>

#include "Capture.h"
#include "Show.h"
#include "Play.h"

class Stream : public Capture {
Q_OBJECT

public:
    explicit Stream(QTcpSocket *socket, QObject *parent = nullptr);

    ~Stream() override;

    void close();

signals:

    void changed();

    void connected(int width, int height);

    void voice(QByteArray);

public slots:

    void onReadyRead();

    void onWriteReady(const Frame::Refer& frame);

    void onVoice(const QByteArray&);

protected:

private:

    class Reader;

    typedef Reader (Stream::*Functor)();

    class Reader {
    public:
        Reader(Functor reader) : mReader(reader) {}

        operator Functor() const { return mReader; }

    private:
        Functor mReader;
    };

    Functor mReader = &Stream::readBegin;

    inline void read() { if (mReader) mReader = (this->*mReader)(); }

    inline bool isEnough(qint64 bytes) { return bytes > mSocket->bytesAvailable(); }

    Reader readBegin();

    Reader readToken();

    Reader readType();

    Reader readWidth();

    Reader readHeight();

    Reader readLength();

    Reader readPayload();

    Reader readId();

    Reader readFlag();

    QDataStream mStream;

    QTcpSocket *mSocket;

    Frame *mFrame{};

    Show *mShow{};

    Play *mPlay{};
};


#endif //CAMERA_STREAM_H
