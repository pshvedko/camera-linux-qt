//
// Created by shved@mail.ru on 12.08.2020.
//

#ifndef CAMERA_AUDIO_H
#define CAMERA_AUDIO_H

#include <alsa/asoundlib.h>

class Audio {
public:

    Audio(const char device[], uint32_t rate, uint32_t channels,
          snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE,
          snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE,
          snd_pcm_access_t access = SND_PCM_ACCESS_RW_INTERLEAVED);

    virtual ~Audio();

protected:

    bool record(short *buffer, snd_pcm_uframes_t size);
    bool play(short *buffer, snd_pcm_uframes_t size);

private:

    snd_pcm_t *mPcm{};
};


#endif //CAMERA_AUDIO_H
