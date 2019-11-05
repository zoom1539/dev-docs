#ifndef JI_UTILS
#define JI_UTILS

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

/**
 * 获取文件大小
 *
 * @param ifs 打开的文件
 * @return 文件大小
 */
size_t getFileLen(std::ifstream &ifs);

/**
 * 在图上画矩形框，并在框顶部画文字
 *
 * @param img   需要画的图
 * @param leftTopRightBottomRect    矩形框(x, y, width, height)，其中(x, y)是左上角坐标，(width, height)是框的宽高
 * @param text  需要画的文字
 * @param rectLineThickness 矩形框的线宽度
 * @param rectColor     矩形框的颜色
 * @param fontHeight    字体高度
 * @param textColor 字体颜色
 * @param textBg    字体背景颜色
 */
void drawRectAndText(cv::Mat &img, cv::Rect &leftTopRightBottomRect, const std::string &text, int rectLineThickness,
                     cv::Scalar rectColor, int fontHeight, cv::Scalar textColor, cv::Scalar textBg);

void drawPolygon(cv::Mat &img, std::vector<std::vector<cv::Point> > polygons, cv::Scalar color, int thickness=3);
#endif  // JI_UTILS