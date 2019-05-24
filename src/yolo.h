#ifndef __YOLO_H__
#define __YOLO_H__

#include "opencv2/opencv.hpp"

#include "cuda_runtime.h"
#include "curand.h"
#include "cublas_v2.h"
#include "cudnn.h"


#define GPU
#define CUDNN

extern "C"
{
#include "darknet.h"
}

typedef struct CvTarget
{
    cv::Rect rect;                      //�~[��| ~G��~F��~M置
    cv::Mat target_img;         //�~[��| ~G��~F�~@~I�~[�
    cv::Point center;           //�~[��| ~G��~F中��~C�~]~P�| ~G
    float confidence;           //�~[��| ~G��~F�~O�信度
    int class_type;         //��~@��~K�| ~G签索��~U
    CvTarget()
    {
        rect.x = 0;
        rect.y = 0;
        rect.height = 0;
        rect.width = 0;
        center.x = 0;
        center.y = 0;
        confidence = 0;
    }
}CvTarget;
typedef std::vector<CvTarget> VEC_TARGET;


class yolo
{
public:
	yolo(const char *cfgfile, const char *weightfile, const char* labelfile, float thresh, int gpu_id = 0);
	~yolo();

	int detect(cv::Mat& img, VEC_TARGET& vecTarget);

private:
  	bool init_table();
        image mat_to_img(cv::Mat& mat);
private:
	network *m_net;
	float m_thresh;
	char **m_label_names;
	float m_table[256];
};

#endif //__YOLO_H__
