#include <opencv2/opencv.hpp>
#include "tools.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: ./main <image_path>" << std::endl;
        return -1;
    }

    cv::Mat image = cv::imread(argv[1]);
    if (image.empty()) {
        std::cout << "Could not open image" << std::endl;
        return -1;
    }

    cv::Mat result;
    auto [scale, x, y] = resizeAndPad(image, result);

    std::cout << "Scale: " << scale << std::endl;
    std::cout << "Offset X: " << x << std::endl;
    std::cout << "Offset Y: " << y << std::endl;

    cv::imshow("Result", result);
    cv::waitKey(0);

    return 0;
}