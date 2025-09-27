// 防止头文件重复包含
#ifndef MY_CAMERA_HPP
#define MY_CAMERA_HPP

// 包含需要的头文件
#include "hikrobot/include/MvCameraControl.h"
#include <opencv2/opencv.hpp>
// 相机类
class myCamera {
public:
    // 构造函数：初始化相机
    myCamera();
    // 析构函数：释放相机资源
    ~myCamera();
    // 读取一帧图像，成功返回true，失败返回false，图像通过参数传出
    bool read(cv::Mat& img);

private:
    // 私有变量，都以_结尾
    void* handle_;          // 相机句柄
    bool is_opened_;        // 相机是否打开标志
    MV_FRAME_OUT raw_frame_;// 原始帧数据
};

#endif // MY_CAMERA_HPP