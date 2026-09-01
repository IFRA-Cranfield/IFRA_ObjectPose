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
# IFRA-Cranfield (2023) ObjectPose Plugin for Gazebo Harmonic / GZ Sim simulation. URL: https://github.com/IFRA-Cranfield/IFRA_ObjectPose.

*/

// INCLUDE -> HPP header file:
#include "ros2_objectpose/ros2_objectpose_plugin.hpp"
// INCLUDE -> Gazebo Harmonic:
#include <gz/sim/EntityComponentManager.hh>
#include <gz/sim/Util.hh>
#include <gz/math/Pose3.hh>
#include <gz/plugin/Register.hh>

namespace ros2_objectpose
{

Ros2ObjectPose::~Ros2ObjectPose()
{
  // Only shutdown if we were the ones who initialized rclcpp
  if (owns_rclcpp_ && rclcpp::ok())
    rclcpp::shutdown();
}

void Ros2ObjectPose::Configure(const gz::sim::Entity &entity,
                               const std::shared_ptr<const sdf::Element> &sdf,
                               gz::sim::EntityComponentManager &ecm,
                               gz::sim::EventManager &)
{
  model_ = gz::sim::Model(entity);
  targetEntity_ = entity;
  targetIsModel_ = true;

  if (sdf)
  {
    if (sdf->HasElement("ros"))
    {
      auto rosElem = sdf->FindElement("ros");
      if (rosElem && rosElem->HasElement("namespace"))
        ns_ = rosElem->Get<std::string>("namespace");
    }
    if (sdf->HasElement("topic"))
      topic_ = sdf->Get<std::string>("topic");
    if (sdf->HasElement("frame_id"))
      frame_id_ = sdf->Get<std::string>("frame_id");
    if (sdf->HasElement("object_name"))
      object_name_ = sdf->Get<std::string>("object_name");

    if (sdf->HasElement("link_name"))
    {
      const auto linkName = sdf->Get<std::string>("link_name");
      auto linkEntity = model_.LinkByName(ecm, linkName);
      if (linkEntity != gz::sim::kNullEntity)
      {
        targetEntity_ = linkEntity;
        targetIsModel_ = false;
      }
      else
      {
        gzdbg << "[Ros2ObjectPose] link_name '" << linkName
              << "' not found; falling back to model pose.\n";
      }
    }
  }

  if (object_name_.empty())
  {
    auto nameComp = ecm.Component<gz::sim::components::Name>(targetEntity_);
    object_name_ = nameComp ? nameComp->Data() : std::string("object");
  }

  // Ensure a ROS 2 context exists
  if (!rclcpp::ok())
  {
    int argc = 0; char **argv = nullptr;
    rclcpp::init(argc, argv);
    owns_rclcpp_ = true;
  }

  // Unique node name per entity id to avoid collisions
  const std::string node_name = "ifra_object_pose_" +
      std::to_string(static_cast<unsigned long long>(targetEntity_));

  rclcpp::NodeOptions opts;
  opts.append_parameter_override("use_sim_time", true);
  node_ = std::make_shared<rclcpp::Node>(node_name, ns_, opts);

  pub_ = node_->create_publisher<objectpose_msgs::msg::ObjectPose>(
      topic_, rclcpp::QoS(10));

  ecm.CreateComponent(targetEntity_, gz::sim::components::Pose());
}

void Ros2ObjectPose::PostUpdate(const gz::sim::UpdateInfo &,
                                const gz::sim::EntityComponentManager &ecm)
{
  if (!pub_) return;

  const auto p = gz::sim::worldPose(targetEntity_, ecm);

  objectpose_msgs::msg::ObjectPose msg;
  msg.objectname = object_name_;
  msg.x  = p.Pos().X();
  msg.y  = p.Pos().Y();
  msg.z  = p.Pos().Z();
  msg.qx = p.Rot().X();
  msg.qy = p.Rot().Y();
  msg.qz = p.Rot().Z();
  msg.qw = p.Rot().W();

  pub_->publish(msg);
}

} // namespace ros2_objectpose

GZ_ADD_PLUGIN(
  ros2_objectpose::Ros2ObjectPose,
  gz::sim::System,
  ros2_objectpose::Ros2ObjectPose::ISystemConfigure,
  ros2_objectpose::Ros2ObjectPose::ISystemPostUpdate)

GZ_ADD_PLUGIN_ALIAS(ros2_objectpose::Ros2ObjectPose, "ros2_objectpose_plugin")
