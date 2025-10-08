#include "buff_solver.hpp"
#include <opencv2/calib3d.hpp>
#include <iostream>

namespace auto_buff
{

Buff_Solver::Buff_Solver()
{
    initWorldPoints();
}

void Buff_Solver::initWorldPoints()
{
    float half_w = armor_width_ / 2.0f;
    float half_h = armor_height_ / 2.0f;
    
    world_points_.clear();
    world_points_.emplace_back(-half_w, -half_h, 0.0f);
    world_points_.emplace_back( half_w, -half_h, 0.0f);
    world_points_.emplace_back( half_w,  half_h, 0.0f);
    world_points_.emplace_back(-half_w,  half_h, 0.0f);
}

void Buff_Solver::setCameraParams(const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs)
{
    camera_matrix_ = camera_matrix.clone();
    dist_coeffs_ = dist_coeffs.clone();
}

bool Buff_Solver::solvePnP(const std::vector<cv::Point2f>& image_points, 
                          cv::Mat& rvec, cv::Mat& tvec)
{
    if (image_points.size() != 4) {
        return false;
    }
    
    if (camera_matrix_.empty() || dist_coeffs_.empty()) {
        return false;
    }
    
    bool success = cv::solvePnP(world_points_, image_points, 
                               camera_matrix_, dist_coeffs_, 
                               rvec, tvec, false, cv::SOLVEPNP_IPPE);
    
    if (success) {
        current_center_ = cv::Point3f(tvec.at<double>(0), 
                                     tvec.at<double>(1), 
                                     tvec.at<double>(2));
        rotation_center_ = calculateRotationCenter(current_center_);
    }
    
    return success;
}

cv::Point3f Buff_Solver::calculateRotationCenter(const cv::Point3f& current_center)
{
    centers_history_.push_back(current_center);
    if (centers_history_.size() > max_history_size_) {
        centers_history_.pop_front();
    }
    
    if (centers_history_.size() < 10) {
        return cv::Point3f(0, 0, 0);
    }
    
    std::vector<cv::Point2f> xy_points;
    for (const auto& center : centers_history_) {
        xy_points.emplace_back(center.x, center.y);
    }
    
    cv::Point2f estimated_center;
    float radius;
    cv::minEnclosingCircle(xy_points, estimated_center, radius);
    
    float avg_z = 0.0f;
    for (const auto& center : centers_history_) {
        avg_z += center.z;
    }
    avg_z /= centers_history_.size();
    
    return cv::Point3f(estimated_center.x, estimated_center.y, avg_z);
}

cv::Point3f Buff_Solver::getCurrentCenter() const
{
    return current_center_;
}

cv::Point3f Buff_Solver::getRotationCenter() const
{
    return rotation_center_;
}

}
