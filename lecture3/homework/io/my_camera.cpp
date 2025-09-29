#include "my_camera.hpp"

// 构造函数实现：打开并初始化相机
myCamera::myCamera() {
    // 初始化为未打开状态
    is_opened_ = false;
    handle_ = nullptr;

    // 枚举设备
    MV_CC_DEVICE_INFO_LIST device_list;
    int ret = MV_CC_EnumDevices(MV_USB_DEVICE, &device_list);
    if (ret != MV_OK || device_list.nDeviceNum == 0) {
        return; // 枚举设备失败或无设备
    }

    // 创建相机句柄
    ret = MV_CC_CreateHandle(&handle_, device_list.pDeviceInfo[0]);
    if (ret != MV_OK) {
        return;
    }

    // 打开相机
    ret = MV_CC_OpenDevice(handle_);
    if (ret != MV_OK) {
        return;
    }

    // 设置相机参数
    MV_CC_SetEnumValue(handle_, "BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_CONTINUOUS);
    MV_CC_SetEnumValue(handle_, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetEnumValue(handle_, "GainAuto", MV_GAIN_MODE_OFF);
    MV_CC_SetFloatValue(handle_, "ExposureTime",8000 );
    MV_CC_SetFloatValue(handle_, "Gain", 16.9);
    MV_CC_SetFrameRate(handle_, 60);

    // 开始取流
    ret = MV_CC_StartGrabbing(handle_);
    if (ret == MV_OK) {
        is_opened_ = true; // 所有步骤成功，标记相机已打开
    }
}

// 析构函数实现：关闭相机并释放资源
myCamera::~myCamera() {
    if (is_opened_) {
        // 停止取流
        MV_CC_StopGrabbing(handle_);
        // 关闭设备
        MV_CC_CloseDevice(handle_);
        // 销毁句柄
        MV_CC_DestroyHandle(handle_);
    }
}

// 图像转换辅助函数（私有，只在类内部使用）
cv::Mat transfer(MV_FRAME_OUT& raw) {
    // 定义像素转换参数
    MV_CC_PIXEL_CONVERT_PARAM cvt_param;

    // 创建OpenCV图像矩阵，使用原始数据的地址和尺寸
    cv::Mat img(cv::Size(raw.stFrameInfo.nWidth, raw.stFrameInfo.nHeight), CV_8U, raw.pBufAddr);

    // 设置转换参数的宽度和高度
    cvt_param.nWidth = raw.stFrameInfo.nWidth;
    cvt_param.nHeight = raw.stFrameInfo.nHeight;

    // 设置原始数据的地址和长度
    cvt_param.pSrcData = raw.pBufAddr;
    cvt_param.nSrcDataLen = raw.stFrameInfo.nFrameLen;
    cvt_param.enSrcPixelType = raw.stFrameInfo.enPixelType;

    // 设置目标数据的地址和缓冲区大小
    cvt_param.pDstBuffer = img.data;
    cvt_param.nDstBufferSize = img.total() * img.elemSize();
    cvt_param.enDstPixelType = PixelType_Gvsp_BGR8_Packed;

    // 获取原始数据的像素类型
    auto pixel_type = raw.stFrameInfo.enPixelType;

    // 像素类型映射表：不同相机原始格式对应OpenCV转换格式
    const static std::unordered_map<MvGvspPixelType, cv::ColorConversionCodes> type_map = {
        {PixelType_Gvsp_BayerGR8, cv::COLOR_BayerGR2RGB},
        {PixelType_Gvsp_BayerRG8, cv::COLOR_BayerRG2RGB},
        {PixelType_Gvsp_BayerGB8, cv::COLOR_BayerGB2RGB},
        {PixelType_Gvsp_BayerBG8, cv::COLOR_BayerBG2RGB}
    };

    // 转换为RGB格式
    pixel_type = raw.stFrameInfo.enPixelType;
    cv::cvtColor(img, img, type_map.at(pixel_type));
    
    return img;
}

// 读取图像函数实现
bool myCamera::read(cv::Mat& img) {
    if (!is_opened_) {
        return false; // 相机未打开，返回失败
    }

    // 获取一帧图像（超时时间100毫秒）
    int ret = MV_CC_GetImageBuffer(handle_, &raw_frame_, 100);
    if (ret != MV_OK) {
        return false; // 获取图像失败
    }

    // 转换为OpenCV格式
    img = transfer(raw_frame_);

    // 释放原始帧缓存
    MV_CC_FreeImageBuffer(handle_, &raw_frame_);

    return true; // 成功获取图像
}