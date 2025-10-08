#include "tasks/buff_detector.hpp"
#include "tasks/buff_solver.hpp"
#include "io/camera.hpp"
#include "tools/img_tools.hpp"
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class PlotJugglerSender
{
public:
    PlotJugglerSender(const std::string& ip = "127.0.0.1", int port = 9870)
        : server_ip_(ip), server_port_(port)
    {
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(server_port_);
        server_addr_.sin_addr.s_addr = inet_addr(server_ip_.c_str());
    }
    
    void sendData(const nlohmann::json& data)
    {
        std::string json_str = data.dump();
        sendto(sockfd_, json_str.c_str(), json_str.length(), 0,
               (struct sockaddr*)&server_addr_, sizeof(server_addr_));
    }

private:
    int sockfd_;
    std::string server_ip_;
    int server_port_;
    struct sockaddr_in server_addr_;
};

int main()
{
    io::Camera camera(2.5, 16.9, "2bdf:0001");
    std::chrono::steady_clock::time_point timestamp;
    
    auto_buff::Buff_Detector detector;
    auto_buff::Buff_Solver solver;
    
    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 
        1286.307063384126, 0, 645.34450819155256,
        0, 1288.1400736562441, 483.6163720308021,
        0, 0, 1);
        
    cv::Mat dist_coeffs = (cv::Mat_<double>(1, 5) << 
        -0.47562935060124745, 0.21831745829617311, 
        0.0004957613589406044, -0.00034617769548693592, 0.0);
    
    solver.setCameraParams(camera_matrix, dist_coeffs);
    
    PlotJugglerSender plot_sender;
    
    while (true) {
        cv::Mat img;
        camera.read(img, timestamp);
        
        auto fanblades = detector.detect(img);
        
        if (!fanblades.empty()) {
            const auto& blade = fanblades[0];
            
            if (blade.points.size() >= 4) {
                std::vector<cv::Point2f> image_points;
                for (int i = 0; i < 4; ++i) {
                    image_points.push_back(blade.points[i]);
                }
                
                cv::Mat rvec, tvec;
                if (solver.solvePnP(image_points, rvec, tvec)) {
                    cv::Point3f current_center = solver.getCurrentCenter();
                    cv::Point3f rotation_center = solver.getRotationCenter();
                    
                    nlohmann::json data;
                    auto now = std::chrono::system_clock::now();
                    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                        now.time_since_epoch()).count();
                    
                    data["timestamp"] = timestamp;
                    data["current_center_x"] = current_center.x;
                    data["current_center_y"] = current_center.y;
                    data["current_center_z"] = current_center.z;
                    data["rotation_center_x"] = rotation_center.x;
                    data["rotation_center_y"] = rotation_center.y;
                    data["rotation_center_z"] = rotation_center.z;
                    
                    plot_sender.sendData(data);
                    
                    cv::putText(img, "Center: " + std::to_string(current_center.x) + ", " +
                                std::to_string(current_center.y),
                                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                                cv::Scalar(0, 255, 0), 2);
                    
                    cv::putText(img, "R-Center: " + std::to_string(rotation_center.x) + ", " +
                                std::to_string(rotation_center.y),
                                cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                                cv::Scalar(0, 255, 255), 2);
                }
            }
            
            for (const auto& blade : fanblades) {
                tools::draw_point(img, blade.center, cv::Scalar(0, 255, 0), 5);
                for (const auto& point : blade.points) {
                    tools::draw_point(img, point, cv::Scalar(255, 0, 0), 3);
                }
            }
        }
        
        cv::imshow("Buff Detection", img);
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
    
    return 0;
}