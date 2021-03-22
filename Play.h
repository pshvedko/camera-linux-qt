//
// Created by shved@mail.ru on 12.08.2020.
//

#ifndef CAMERA_PLAY_H
#define CAMERA_PLAY_H


#include <opus/opus.h>

#include "Audio.h"
#include "Loop.h"

class Play : public Loop, Audio {
Q_OBJECT

public:

    Play(const char *device, uint32_t channels, QObject *parent);

    ~Play() override;

signals:

    void voice(QByteArray);

protected:

    void loop(Frame *frame, Queue *queue) override;

private:

    OpusDecoder *mCodec{};
    timespec mTime{};
    int mChannels;
    int mBuffSize;
    bool mSkip;
};


#endif //CAMERA_PLAY_H
