#include "tools.hpp"
#include <tuple>

std::tuple<double, int, int> resizeAndPad(const cv::Mat &input, cv::Mat &output, int size) {
    output = cv::Mat(size, size, input.type(), cv::Scalar(0, 0, 0));
    
    double scale = std::min(static_cast<double>(size) / input.cols,
                           static_cast<double>(size) / input.rows);
    
    int newWidth = static_cast<int>(input.cols * scale);
    int newHeight = static_cast<int>(input.rows * scale);
    
    cv::Mat resized;
    cv::resize(input, resized, cv::Size(newWidth, newHeight));
    
    int x = (size - newWidth) / 2;
    int y = (size - newHeight) / 2;
    
    cv::Mat roi(output, cv::Rect(x, y, newWidth, newHeight));
    resized.copyTo(roi);
    
    return std::make_tuple(scale, x, y);
}