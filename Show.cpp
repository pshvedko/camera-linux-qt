//
// Created by shved@mail.ru on 05.08.2020.
//

#include <QDebug>

#include <vpx/vp8dx.h>
#include <vpx/vpx_decoder.h>

#include <libyuv.h>

#include "Show.h"

using namespace libyuv;

Show::Show(const char device[], unsigned int width, unsigned int height, QObject *parent) :
        Loop(parent), Video(device, width, height, V4L2_BUF_TYPE_VIDEO_OUTPUT) {
    vpx_codec_dec_init(&mCodec, &vpx_codec_vp8_dx_algo, nullptr, 0);
    unsigned int c[8] = {0x00000000, 0x00ff0000, 0x0000ff00, 0x000000ff,
                         0x0000ffff, 0x00ff00ff, 0x00ffff00, 0x00ffffff};
    unsigned char a[width * height * 4], *b = a;
    for (unsigned int d = width / 8, h = 0; h < height; ++h)
        for (int w = 0; w < width; ++w, b += 4)
            memcpy(b, c + w / d, 4);
    auto image = vpx_img_alloc(nullptr, VPX_IMG_FMT_I420, width, height, 64);
    ConvertToI420(a, 0,
                  image->planes[VPX_PLANE_Y], image->stride[VPX_PLANE_Y],
                  image->planes[VPX_PLANE_U], image->stride[VPX_PLANE_U],
                  image->planes[VPX_PLANE_V], image->stride[VPX_PLANE_V],
                  0, 0, image->d_w, image->d_h,
                  image->stride[VPX_PLANE_Y],
                  image->d_h, kRotate0, FOURCC_ARGB);
    ConvertFromI420(
            image->planes[VPX_PLANE_Y], image->stride[VPX_PLANE_Y],
            image->planes[VPX_PLANE_U], image->stride[VPX_PLANE_U],
            image->planes[VPX_PLANE_V], image->stride[VPX_PLANE_V],
            a, 0, image->d_w, image->d_h,
            FOURCC_I420);
    vpx_img_free(image);
    play(a, sizeof(a) / 4 * 3 / 2);
}

Show::~Show() {
    done();
    quit();
    wait();
    vpx_codec_destroy(&mCodec);
}

void Show::loop(Frame *frame, Queue *queue) {
    Frame::Re re(frame);
    if (frame->mType != Frame::Video)
        return;
    auto err = vpx_codec_decode(&mCodec, frame->mBytes.mData, frame->mBytes.mLength, nullptr, 0);
    if (err != VPX_CODEC_OK)
        return;
    vpx_codec_iter_t iter = nullptr;
    for (;;) {
        auto image = vpx_codec_get_frame(&mCodec, &iter);
        if (!image)
            break;
        unsigned char b[image->d_w * image->d_h * 3 / 2];
        ConvertFromI420(
                image->planes[VPX_PLANE_Y], image->stride[VPX_PLANE_Y],
                image->planes[VPX_PLANE_U], image->stride[VPX_PLANE_U],
                image->planes[VPX_PLANE_V], image->stride[VPX_PLANE_V],
                b, 0, image->d_w, image->d_h, FOURCC_I420);
        play(b, sizeof(b));
    }
}
