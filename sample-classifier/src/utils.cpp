//
// Created by hrh on 2019-09-03.
//

#ifndef JI_UTILS_CPP
#define JI_UTILS_CPP

#include <darknet.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types_c.h>

/**
 * 将cv:Mat转换成darknet定义的格式
 * @param m
 * @return
 */
image mat_to_image(const cv::Mat &m) {
    int h = m.size().height;
    int w = m.size().width;
    int c = m.channels();
    image im = make_image(w, h, c);
    unsigned char *data = m.data;
    int step = m.step;
    int i, j, k;

    for(i = 0; i < h; ++i){
        for(k= 0; k < c; ++k){
            for(j = 0; j < w; ++j){
                im.data[k*w*h + i*w + j] = data[i*step + j*c + k]/255.;
            }
        }
    }

    rgbgr_image(im);
    return im;
}

uchar *cv_mat_to_rgb24(cv::Mat *m) {
    int width = m->cols;
    int height = m->rows;
    int channels = 3;
    uchar *rgb24 = (uchar *)malloc(width * height * channels);
    for (int c = 0; c < channels; ++c) {
        for (int w = 0; w < width; ++w) {
            for (int h = 0; h < height; ++h) {
                rgb24[h + w * width + c * width * height] = m->data[]
            }
        }
    }
}

#endif //JI_UTILS_CPP
