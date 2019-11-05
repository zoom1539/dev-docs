#include <ji_utils.h>
#include <opencv2/freetype.hpp>
#include <iostream>
#include <fstream>

size_t getFileLen(std::ifstream &ifs) {
    int origPos = ifs.tellg();
    ifs.seekg(0, std::fstream::end);
    size_t len = ifs.tellg();
    ifs.seekg(origPos);
    return len;
}

void drawRectAndText(cv::Mat &img, cv::Rect &leftTopRightBottomRect, const std::string &text,
                    int rectLineThickness, cv::Scalar rectColor, int fontHeight,
                    cv::Scalar textColor, cv::Scalar textBg) {
    // Draw rectangle
    cv::Point rectLeftTop(leftTopRightBottomRect.x, leftTopRightBottomRect.y);
    cv::rectangle(img, leftTopRightBottomRect, rectColor, rectLineThickness, cv::LINE_AA, 0);

    // Draw text and text background
    cv::Ptr<cv::freetype::FreeType2> ft2;
    int baseline = 0;
    ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData("/usr/local/ev_sdk/lib/fonts/DejaVuSans.ttf", 0);

    cv::Size textSize = ft2->getTextSize(text, fontHeight, -1, &baseline);
    cv::Point textLeftBottom(leftTopRightBottomRect.x, leftTopRightBottomRect.y);
    textLeftBottom -= cv::Point(0, rectLineThickness);
    textLeftBottom -= cv::Point(0, baseline);   // (left, bottom) of text
    cv::Point textLeftTop(textLeftBottom.x, textLeftBottom.y - textSize.height);    // (left, top) of text
    // Draw text background
    cv::rectangle(img, textLeftTop, textLeftTop + cv::Point(textSize.width, textSize.height + baseline), textBg, cv::FILLED);
    // Draw text
    ft2->putText(img, text, textLeftBottom, fontHeight, textColor, -1, cv::LINE_AA, true);
}

void drawPolygon(cv::Mat &img, std::vector<std::vector<cv::Point> > polygons, cv::Scalar color, int thickness) {
    for ( size_t i = 0; i < polygons.size(); i++)
    {
        const cv::Point* p = &polygons[i][0];
        int n = (int)polygons[i].size();
        cv::polylines(img, &p, &n, 1, true, color, thickness, cv::LINE_AA);
    }
}

