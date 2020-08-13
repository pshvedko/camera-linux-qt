//
// Created by shved@mail.ru on 12.08.2020.
//

#ifndef CAMERA_VIDEO_H
#define CAMERA_VIDEO_H


#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#include <cstdint>


class Video {
public:

    Video(const char device[], unsigned int width, unsigned int height,
          v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
          uint32_t format = V4L2_PIX_FMT_YUV420,
          v4l2_field field = V4L2_FIELD_NONE);

    virtual ~Video();

protected:

    bool play(const void *buf, size_t n) const;

private:

    int mFd;

};


#endif //CAMERA_VIDEO_H
