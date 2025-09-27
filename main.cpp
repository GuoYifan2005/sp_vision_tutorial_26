#include "io/camera.hpp"
#include "tasks/yolo.hpp"
#include "opencv2/opencv.hpp"
#include "tools/img_tools.hpp"

// develop things
#include <iostream>
int main()
{
    // std::string camera_config_path = "./configs/camera.yaml";
    // io::Camera camera(camera_config_path);

    cv::VideoCapture capture("./assets/demo/demo.avi"); // 读取本地视频
    
    std::string config_path = "./configs/yolo.yaml";
    auto_aim::YOLO yolo(config_path, false);

    while (1) {
        // 读取相机
        cv::Mat img;
        // std::chrono::steady_clock::time_point timestamp;
        // camera.read(img, timestamp);

        capture >> img;
        
        // 识别
        auto armors = yolo.detect(img);

        // 绘制
        for (auto & armor : armors) {
            tools::draw_points(img, armor.points, cv::Scalar(0,125,255), 4);    // (0,125,255)-橙色
        }

        // 显示
        cv::resize(img, img , cv::Size(640, 480));
        cv::imshow("img", img);
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
}