#ifndef AUTO_BUFF__SOLVER_HPP
#define AUTO_BUFF__SOLVER_HPP
#include <opencv2/opencv.hpp>
#include <vector>
#include <deque>
#include "buff_type.hpp"
namespace auto_buff
{

class Buff_Solver
{
public:
    Buff_Solver();
    
    // PnP解算方法
    bool solvePnP(const std::vector<cv::Point2f>& image_points, 
                 cv::Mat& rvec, cv::Mat& tvec);
    
    // 计算旋转中心 
    cv::Point3f calculateRotationCenter(const cv::Point3f& current_center);
    
    // 获取解算结果
    cv::Point3f getCurrentCenter() const ;
    cv::Point3f getRotationCenter() const;
    
    // 设置相机参数
    void setCameraParams(const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs);

private:
    // 世界坐标点定义（基于能量机关物理尺寸）
    std::vector<cv::Point3f> world_points_;
    
    // 相机参数
    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_;
    
    // 解算结果
    cv::Point3f current_center_;
    cv::Point3f rotation_center_;
    
    // 历史数据用于旋转中心计算
    std::deque<cv::Point3f> centers_history_;
    const size_t max_history_size_ = 50;
    
    // 能量机关物理尺寸（单位：米）
    const float armor_width_ = 0.23f;    // 装甲板宽度
    const float armor_height_ = 0.12f;   // 装甲板高度
    
    // 初始化世界坐标点
    void initWorldPoints();
};

}  // namespace auto_buff
#endif  // SOLVER_HPP