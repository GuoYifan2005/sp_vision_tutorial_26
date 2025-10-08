cmake_minimum_required(VERSION 3.16)

add_library(io STATIC
    hikrobot/hikrobot.cpp
    camera.cpp
)

target_include_directories(io PUBLIC hikrobot/include)

# 修正：使用 target_link_directories 来添加库目录
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_link_directories(io PUBLIC hikrobot/lib/amd64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    target_link_directories(io PUBLIC hikrobot/lib/arm64)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}!")
endif()
target_link_directories(io PUBLIC /opt/MVS/lib/64)
# 链接具体的库文件
target_link_libraries(io MvCameraControl usb-1.0 ${OpenCV_LIBS})