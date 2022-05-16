//
// Created by shved on 23.03.2021.
//

#ifndef CAMERA_PULSE_H
#define CAMERA_PULSE_H

#include <pulse/pulseaudio.h>

class Pulse {
public:
    Pulse();

    virtual ~Pulse();

    explicit operator pa_mainloop *() const { return mLoop; }

    void Do(pa_context *pContext);

protected:
    void availableSinks();

    void setSink(int id);

private:
    pa_mainloop *mLoop{};
    pa_context_notify_cb_t mCall{};

    void perform(pa_context_notify_cb_t f);
};


#endif //CAMERA_PULSE_H
