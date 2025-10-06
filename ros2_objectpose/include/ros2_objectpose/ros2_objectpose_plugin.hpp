/*

# ===================================== COPYRIGHT ===================================== #
#                                                                                       #
#  IFRA (Intelligent Flexible Robotics and Assembly) Group, CRANFIELD UNIVERSITY        #
#  Created on behalf of the IFRA Group at Cranfield University, United Kingdom          #
#  E-mail: IFRA@cranfield.ac.uk                                                         #
#                                                                                       #
#  Licensed under the Apache-2.0 License.                                               #
#  You may not use this file except in compliance with the License.                     #
#  You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0  #
#                                                                                       #
#  Unless required by applicable law or agreed to in writing, software distributed      #
#  under the License is distributed on an "as-is" basis, without warranties or          #
#  conditions of any kind, either express or implied. See the License for the specific  #
#  language governing permissions and limitations under the License.                    #
#                                                                                       #
#  IFRA Group - Cranfield University                                                    #
#  AUTHORS: Mikel Bueno Viso - Mikel.Bueno-Viso@cranfield.ac.uk                         #
#           Dr. Seemal Asif  - s.asif@cranfield.ac.uk                                   #
#           Prof. Phil Webb  - p.f.webb@cranfield.ac.uk                                 #
#                                                                                       #
#  Date: June, 2023.                                                                    #
#                                                                                       #
# ===================================== COPYRIGHT ===================================== #

# ======= CITE OUR WORK ======= #
# You can cite our work with the following statement:
# IFRA-Cranfield (2023) ObjectPose Plugin for ROS2-Gazebo Simulation. URL: https://github.com/IFRA-Cranfield/IFRA_ObjectPose.

*/

#ifndef ROS2_OBJECTPOSE_PLUGIN_HPP
#define ROS2_OBJECTPOSE_PLUGIN_HPP

#include <memory>
#include <thread>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <objectpose_msgs/msg/object_pose.hpp>

#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/sim/Link.hh>
#include <gz/sim/components/Pose.hh>
#include <gz/sim/components/Name.hh>
#include <gz/plugin/Register.hh>

namespace ros2_objectpose
{

class Ros2ObjectPose :
  public gz::sim::System,
  public gz::sim::ISystemConfigure,
  public gz::sim::ISystemPostUpdate
{
public:
  Ros2ObjectPose() = default;
  ~Ros2ObjectPose() override;

  // Called once when the plugin is loaded:
  void Configure(const gz::sim::Entity &entity,
                 const std::shared_ptr<const sdf::Element> &sdf,
                 gz::sim::EntityComponentManager &ecm,
                 gz::sim::EventManager &/*eventMgr*/) override;

  // Called after each simulation iteration:
  void PostUpdate(const gz::sim::UpdateInfo &info,
                  const gz::sim::EntityComponentManager &ecm) override;

private:

  gz::sim::Model model_{gz::sim::kNullEntity};
  gz::sim::Entity targetEntity_{gz::sim::kNullEntity};
  bool targetIsModel_{true};

  // ROS 2:
  std::string ns_{};
  std::string topic_{"ObjectPose"};
  std::string frame_id_{"world"}; 
  std::string object_name_{};
  rclcpp::Node::SharedPtr node_;
  rclcpp::Publisher<objectpose_msgs::msg::ObjectPose>::SharedPtr pub_;
  std::thread spinThread_;
};

} // namespace ros2_objectpose

// Register as a Fortress System plugin :
IGNITION_ADD_PLUGIN(
  ros2_objectpose::Ros2ObjectPose,
  gz::sim::System,
  ros2_objectpose::Ros2ObjectPose::ISystemConfigure,
  ros2_objectpose::Ros2ObjectPose::ISystemPostUpdate)

#endif // ROS2_OBJECTPOSE_PLUGIN_HPP