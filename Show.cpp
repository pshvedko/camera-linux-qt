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
    vpx_img_alloc(&mImage, VPX_IMG_FMT_I420, width, height, 64);
    unsigned char a[width * height * 4];
    memset(a, 0, sizeof(a));
    ConvertToI420(a, 0,
                  mImage.planes[VPX_PLANE_Y], mImage.stride[VPX_PLANE_Y],
                  mImage.planes[VPX_PLANE_U], mImage.stride[VPX_PLANE_U],
                  mImage.planes[VPX_PLANE_V], mImage.stride[VPX_PLANE_V],
                  0, 0, mImage.d_w, mImage.d_h,
                  mImage.stride[VPX_PLANE_Y],
                  mImage.d_h, kRotate0, FOURCC_ARGB);
    ConvertFromI420(
            mImage.planes[VPX_PLANE_Y], mImage.stride[VPX_PLANE_Y],
            mImage.planes[VPX_PLANE_U], mImage.stride[VPX_PLANE_U],
            mImage.planes[VPX_PLANE_V], mImage.stride[VPX_PLANE_V],
            a, 0, mImage.d_w, mImage.d_h,
            FOURCC_I420);
    play(a, sizeof(a) / 4 * 3 / 2);
}

Show::~Show() {
    done();
    quit();
    wait();
    vpx_codec_destroy(&mCodec);
    vpx_img_free(&mImage);
}

void Show::loop(Frame *frame) {
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
                b, 0, image->d_w, image->d_h,
                FOURCC_I420);
        play(b, sizeof(b));
    }
}
