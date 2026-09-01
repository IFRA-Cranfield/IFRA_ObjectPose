cmake_minimum_required(VERSION 3.16)
project(ros2_objectpose)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ROS 2:
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(objectpose_msgs REQUIRED)  # your custom msgs

# Gazebo Harmonic / ROS 2 Jazzy:
find_package(gz-plugin2 REQUIRED COMPONENTS register)
find_package(gz-sim8 REQUIRED)
find_package(gz-math7 REQUIRED)

add_library(ros2_objectpose_plugin SHARED
  src/ros2_objectpose_plugin.cpp
)

target_include_directories(ros2_objectpose_plugin
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ros2_objectpose_plugin PUBLIC cxx_std_17)

# Link against Gazebo Harmonic targets:
target_link_libraries(ros2_objectpose_plugin
  gz-plugin2::gz-plugin2
  gz-sim8::gz-sim8
  gz-math7::gz-math7
)

ament_target_dependencies(ros2_objectpose_plugin
  rclcpp
  objectpose_msgs
)

install(TARGETS ros2_objectpose_plugin LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

ament_environment_hooks(
  environment/gz_sim_system_plugin_path.dsv
  environment/gz_sim_system_plugin_path.sh
)

ament_export_include_directories(include)
ament_export_dependencies(rclcpp objectpose_msgs)

ament_package()
