cmake_minimum_required(VERSION 2.8.3)
project(manas_cv_label)

find_package(catkin REQUIRED COMPONENTS
        roscpp
        cv_bridge
        pcl_ros
)

include_directories(
        ${catkin_INCLUDE_DIRS}
        include
)

add_library(label_tool
        src/label_tool.cpp
        include/label_tool.h
        )
target_link_libraries(label_tool ${catkin_LIBRARIES})

## 2D
add_executable(manas_cv_main
        nodes/main.cpp
)
target_link_libraries(manas_cv_main ${catkin_LIBRARIES} label_tool)

add_executable(manas_trans_pcd_image
        nodes/trans_pcd_image.cpp
        )
target_link_libraries(manas_trans_pcd_image ${catkin_LIBRARIES})

add_executable(manas_listen_rosbag_to_pcd
        nodes/listen_rosbag_to_pcd.cpp
        )
target_link_libraries(manas_listen_rosbag_to_pcd ${catkin_LIBRARIES})


