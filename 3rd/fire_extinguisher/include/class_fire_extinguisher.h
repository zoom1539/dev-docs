#pragma once

#include "opencv2/opencv.hpp"

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
    bool serialize(std::string &wts_path_, const std::string &engine_path_, int class_num_);
    bool init(const std::string &engine_path_);
    bool run(const cv::Mat &img_, 
             const int &fire_extinguisher_num_,
             const int &frame_num_,
             std::vector<cv::Rect> &rects_,
             std::vector<float> &confs_,
             AlertType &alert_);
    

private:
    FireExtinguisher(const FireExtinguisher &);
    const FireExtinguisher &operator=(const FireExtinguisher &);

    class Impl;
    Impl *_impl;
};
