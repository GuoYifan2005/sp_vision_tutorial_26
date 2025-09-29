#include "io/my_camera.hpp"
#include "tasks/yolo.hpp"
#include "opencv2/opencv.hpp"
#include "tools/img_tools.hpp"
#include <fcntl.h>


int main()
{
    // 初始化相机、yolo类
    myCamera camera;
    auto_aim::YOLO yolo("./configs/yolo.yaml");
    cv::Mat frame;
    
    while (1) {
        //调用相机读取图像
        if (!camera.read(frame)) {
            std::cerr << "无法获取图像，退出循环" << std::endl;
            break;
        }

        //调用yolo识别装甲板
        std::list<auto_aim::Armor> armors = yolo.detect(frame);
        for (const auto& armor : armors) {
            // 装甲板关键点已按顺序存储，直接绘制连线（最后连回第一个点）
            std::vector<cv::Point2f> draw_points = armor.points;
            draw_points.push_back(armor.points[0]);  // 闭合矩形
            tools::draw_points(frame, draw_points, cv::Scalar(0, 0, 255), 2);  // 红色线条
            //识别装甲板数字并标在方框上方
            tools::draw_text(frame, std::to_string(armor.name)+ std::to_string(armor.color), armor.center + cv::Point2f(0, -10),
                            cv::Scalar(255, 0, 0), 1.0, 2);   
            
            
        }


        // 显示图像
        cv::resize(frame, frame, cv::Size(640, 480));
        cv::imshow("img", frame);
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
     // 释放窗口资源
    cv::destroyAllWindows();
    return 0;
}