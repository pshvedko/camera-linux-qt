//
// Created by shved@mail.ru on 12.08.2020.
//

#ifndef CAMERA_PLAY_H
#define CAMERA_PLAY_H


#include <opus/opus.h>

#include "Audio.h"
#include "Loop.h"

class Play : public Loop, Audio {
public:

    Play(const char *device, uint32_t channels, QObject *parent);

    ~Play() override;

protected:

    void loop(Frame *frame) override;

private:

    OpusDecoder *mCodec{};
    timespec mTime{};
    int mChannels;
    int mBuffSize;
    bool mSkip;
};


#endif //CAMERA_PLAY_H
