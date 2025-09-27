#include "hikrobot/include/MvCameraControl.h"
#include <opencv2/opencv.hpp>

cv::Mat transfer(MV_FRAME_OUT& raw)
{
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

    // 定义像素类型到OpenCV颜色转换代码的映射
    const static std::unordered_map<MvGvspPixelType, cv::ColorConversionCodes> type_map = {
      {PixelType_Gvsp_BayerGR8, cv::COLOR_BayerGR2RGB}, // BayerGR8 到 RGB 的颜色转换代码
      {PixelType_Gvsp_BayerRG8, cv::COLOR_BayerRG2RGB}, // BayerRG8 到 RGB 的颜色转换代码
      {PixelType_Gvsp_BayerGB8, cv::COLOR_BayerGB2RGB}, // BayerGB8 到 RGB 的颜色转换代码
      {PixelType_Gvsp_BayerBG8, cv::COLOR_BayerBG2RGB}}; // BayerBG8 到 RGB 的颜色转换代码

    // 使用映射中的颜色转换代码进行像素转换
    cv::cvtColor(img, img, type_map.at(pixel_type));
    
    return img;
}
int main()
{ 
    // 打开相机
    // 初始化相机句柄和返回值变量
    void * handle;
    int ret;
    MV_CC_DEVICE_INFO_LIST device_list;
    // 枚举所有USB设备
    ret = MV_CC_EnumDevices(MV_USB_DEVICE, &device_list);
    if (ret != MV_OK) {
      return -1;
    }
  
    // 检查设备列表是否为空
    if (device_list.nDeviceNum == 0) {
      return -1;
    }
  
    // 创建相机句柄并连接到第一个设备
    ret = MV_CC_CreateHandle(&handle, device_list.pDeviceInfo[0]);
    if (ret != MV_OK) {
      return -1;
    }
  
    // 打开相机设备
    ret = MV_CC_OpenDevice(handle);
    if (ret != MV_OK) {
      return -1;
    }
  
    // 设置相机参数
    MV_CC_SetEnumValue(handle, "BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_CONTINUOUS);
    MV_CC_SetEnumValue(handle, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetEnumValue(handle, "GainAuto", MV_GAIN_MODE_OFF);
    MV_CC_SetFloatValue(handle, "ExposureTime", 10000);
    MV_CC_SetFloatValue(handle, "Gain", 20);
    MV_CC_SetFrameRate(handle, 60);
   
    // 读取一帧图像
    ret = MV_CC_StartGrabbing(handle);
    if (ret != MV_OK) {
      return -1;
    }
  
    MV_FRAME_OUT raw;
    unsigned int nMsec = 100;

    // 获取图像缓冲区
    ret = MV_CC_GetImageBuffer(handle, &raw, nMsec);
    if (ret != MV_OK) {
      return -1;
    }

    // 将图像缓冲区转换为OpenCV的Mat对象并显示
    cv::Mat img = transfer(raw);
    cv::imshow("img", img);
    cv::waitKey(0);

    // 释放图像缓冲区
    ret = MV_CC_FreeImageBuffer(handle, &raw);
    if (ret != MV_OK) {
      return -1;
    }
    
    // 关闭相机
    // 停止图像采集
    ret = MV_CC_StopGrabbing(handle);
    if (ret != MV_OK) {
        return -1;
    }

    // 关闭相机设备
    ret = MV_CC_CloseDevice(handle);
    if (ret != MV_OK) {
        return -1;
    }

    // 销毁相机句柄
    ret = MV_CC_DestroyHandle(handle);
    if (ret != MV_OK) {
        return -1;
    }

}
