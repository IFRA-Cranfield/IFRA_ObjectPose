cmake_minimum_required(VERSION 3.16)
project(ros2_objectpose)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ROS 2:
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(objectpose_msgs REQUIRED)  # your custom msgs

# Gazebo Fortress (Ignition):
find_package(ignition-plugin1 REQUIRED COMPONENTS register)
find_package(ignition-gazebo6 REQUIRED)
find_package(ignition-math6 REQUIRED)
find_package(ignition-common4 REQUIRED)

add_library(ros2_objectpose_plugin SHARED
  src/ros2_objectpose_plugin.cpp
)

target_include_directories(ros2_objectpose_plugin
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ros2_objectpose_plugin PUBLIC cxx_std_17)

# Link against Ignition targets for Fortress:
target_link_libraries(ros2_objectpose_plugin
  ignition-plugin1::ignition-plugin1
  ignition-gazebo6::ignition-gazebo6
  ignition-math6::ignition-math6
  ignition-common4::ignition-common4
)

ament_target_dependencies(ros2_objectpose_plugin
  rclcpp
  objectpose_msgs
)

install(TARGETS ros2_objectpose_plugin LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

ament_export_include_directories(include)
ament_export_dependencies(rclcpp objectpose_msgs)

ament_package()