#pragma once

#include "opencv2/opencv.hpp"
#include "WKTParser.h"

typedef enum _AlertType
{
    LostFireExtinguishers = 0,
    NotLostFireExtinguishers
}AlertType;

class FireExtinguisher
{
public:
    explicit FireExtinguisher();
    ~FireExtinguisher();

public:
    bool init(std::string &wts_path_, int class_num_);
    bool run(const cv::Mat &img_, 
             float conf_thres_,
             int frame_num_thres_,
             const std::vector<VectorPoint> &polygon_rois_,
             std::vector<cv::Rect> &rects_,
             std::vector<float> &confs_,
             AlertType &alert_);
    

private:
    FireExtinguisher(const FireExtinguisher &);
    const FireExtinguisher &operator=(const FireExtinguisher &);

    class Impl;
    Impl *_impl;
};
