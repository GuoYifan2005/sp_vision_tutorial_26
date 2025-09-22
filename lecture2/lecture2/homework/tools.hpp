#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <opencv2/opencv.hpp>

std::tuple<double, int, int> resizeAndPad(const cv::Mat &input, cv::Mat &output, int size = 640);

#endif