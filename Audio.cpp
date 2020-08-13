//
// Created by shved@mail.ru on 12.08.2020.
//

#include <QDebug>

#include "Audio.h"

Audio::Audio(const char device[], uint32_t rate, uint32_t channels,
             snd_pcm_stream_t stream,
             snd_pcm_format_t format,
             snd_pcm_access_t access) {
    int err;
    err = snd_pcm_open(&mPcm, device, stream, 0);
    if (err) {
        qDebug() << "snd_pcm_open" << snd_strerror(err);
        return;
    }
    snd_pcm_info_t *info;
    snd_pcm_info_alloca(&info);
    err = snd_pcm_info(mPcm, info);
    if (err) {
        qDebug() << "snd_pcm_info" << snd_strerror(err);
        return;
    }
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    err = snd_pcm_hw_params_any(mPcm, hw_params);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params_any" << snd_strerror(err);
        return;
    }
    err = snd_pcm_hw_params_set_access(mPcm, hw_params, access);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params_set_access" << snd_strerror(err);
        return;
    }
    err = snd_pcm_hw_params_set_format(mPcm, hw_params, format);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params_set_format" << snd_strerror(err);
        return;
    }
    err = snd_pcm_hw_params_set_rate_near(mPcm, hw_params, &rate, nullptr);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params_set_rate_near" << snd_strerror(err);
        return;
    }
    err = snd_pcm_hw_params_set_channels(mPcm, hw_params, channels);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params_set_channels" << snd_strerror(err);
        return;
    }
    snd_pcm_uframes_t period_size = 480;
    err = snd_pcm_hw_params_set_period_size_near(mPcm, hw_params, &period_size, nullptr);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params" << snd_strerror(err);
        return;
    }
    snd_pcm_uframes_t buffer_size = 2 * period_size;
    err = snd_pcm_hw_params_set_buffer_size_near(mPcm, hw_params, &buffer_size);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params" << snd_strerror(err);
        return;
    }
    err = snd_pcm_hw_params(mPcm, hw_params);
    if (err < 0) {
        qDebug() << "snd_pcm_hw_params" << snd_strerror(err);
        return;
    }
    err = snd_pcm_prepare(mPcm);
    if (err < 0) {
        qDebug() << "snd_pcm_prepare" << snd_strerror(err);
        return;
    }
}

bool Audio::record(short *buffer, snd_pcm_uframes_t size) {
    int err = snd_pcm_readi(mPcm, buffer, size);
    if (err < 0)
        snd_pcm_prepare(mPcm);
    return size == err;
}

bool Audio::play(short *buffer, snd_pcm_uframes_t size) {
    int err = snd_pcm_writei(mPcm, buffer, size);
    if (err < 0)
        snd_pcm_prepare(mPcm);
    return size == err;
}

Audio::~Audio() {
    snd_pcm_drain(mPcm);
    snd_pcm_close(mPcm);
}
