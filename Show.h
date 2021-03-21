//
// Created by shved@mail.ru on 05.08.2020.
//

#ifndef CAMERA_SHOW_H
#define CAMERA_SHOW_H

#include <vpx/vpx_image.h>
#include <vpx/vpx_codec.h>

#include "Loop.h"
#include "Video.h"

class Show : public Loop, Video {

public:

    Show(const char device[], unsigned int width, unsigned int height, QObject *parent);

    ~Show() override;

protected:

    void loop(Frame *frame, Queue *queue) override;

private:

    vpx_codec_ctx_t mCodec{};
};


#endif //CAMERA_SHOW_H
