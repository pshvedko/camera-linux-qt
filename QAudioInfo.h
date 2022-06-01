//
// Created by shved on 23.03.2021.
//

#ifndef CAMERA_Q_AUDIO_INFO_H
#define CAMERA_Q_AUDIO_INFO_H

#include <QList>

#include <pulse/pulseaudio.h>

class QAudioInfo {
public:

protected:
    static QList<QAudioInfo> availableSinks();

    void static setSink(int id);

private:
    pa_mainloop *mLoop{};
    pa_context_notify_cb_t mCall{};
};


#endif //CAMERA_Q_AUDIO_INFO_H
