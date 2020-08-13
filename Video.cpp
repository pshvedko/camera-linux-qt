//
// Created by shved@mail.ru on 12.08.2020.
//

#include "Video.h"

Video::Video(const char device[], unsigned int width, unsigned int height, v4l2_buf_type type, uint32_t format,
             v4l2_field field) {
    mFd = open(device, O_RDWR);
    if (mFd < 0)
        return;
    struct v4l2_format v{};
    v.type = type;
    if (ioctl(mFd, VIDIOC_G_FMT, &v) < 0)
        return;
    v.fmt.pix.width = width;
    v.fmt.pix.height = height;
    v.fmt.pix.pixelformat = format;
    v.fmt.pix.field = field;
    if (ioctl(mFd, VIDIOC_S_FMT, &v) < 0)
        return;
}

bool Video::play(const void *buf, size_t n) const {
    return n == write(mFd, buf, n);
}

Video::~Video() {
    close(mFd);
}
