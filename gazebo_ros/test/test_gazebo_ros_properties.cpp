// Copyright 2018 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gazebo/test/ServerFixture.hh>

#include <gazebo_msgs/srv/get_world_properties.hpp>
#include <gazebo_msgs/srv/get_model_properties.hpp>
#include <gazebo_msgs/srv/get_joint_properties.hpp>
#include <gazebo_msgs/srv/get_link_properties.hpp>
#include <gazebo_msgs/srv/get_light_properties.hpp>
#include <gazebo_msgs/srv/get_physics_properties.hpp>
#include <gazebo_msgs/srv/set_joint_properties.hpp>
#include <gazebo_msgs/srv/set_link_properties.hpp>
#include <gazebo_msgs/srv/set_light_properties.hpp>
#include <gazebo_msgs/srv/set_physics_properties.hpp>

#include <geometry_msgs/msg/pose.hpp>
#include <gazebo_msgs/srv/spawn_entity.hpp>
#include <gazebo_ros/conversions/geometry_msgs.hpp>
#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <string>
#include <vector>

#define tol 0.01

class GazeboRosPropertiesTest : public gazebo::ServerFixture
{
public:
  // Documentation inherited
  void SetUp() override;

  void GetModelProperties(
    const std::string & _model_name);

  void GetJointProperties(
    const std::string & _joint_name,
    const double & _damping);

  void GetLinkProperties(
    const std::string & _link_name,
    const bool & _gravity_mode,
    const double & _mass,
    const double & _ixx,
    const double & _ixy,
    const double & _ixz,
    const double & _iyy,
    const double & _iyz,
    const double & _izz);

  void GetLightProperties(
    const std::string & _light_name,
    const ignition::math::Vector4d & _diffuse,
    const double & _attenuation_constant,
    const double & _attenuation_linear,
    const double & _attenuation_quadratic);

  void SetJointProperties(
    const std::string & _joint_name,
    const double & _damping);

  void SetLinkProperties(
    const std::string & _link_name,
    const bool & _gravity_mode,
    const double & _mass,
    const double & _ixx,
    const double & _ixy,
    const double & _ixz,
    const double & _iyy,
    const double & _iyz,
    const double & _izz);

  void SetLightProperties(
    const std::string & _light_name,
    const ignition::math::Vector4d & _diffuse,
    const double & _attenuation_constant,
    const double & _attenuation_linear,
    const double & _attenuation_quadratic);

  gazebo::physics::WorldPtr world_;
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<rclcpp::Client<gazebo_msgs::srv::GetModelProperties>>
  get_model_properties_client_;
  std::shared_ptr<rclcpp::Client<gazebo_msgs::srv::GetJointProperties>>
  get_joint_properties_client_;
  std::shared_ptr<rclcpp::Client<gazebo_msgs::srv::GetLinkProperties>>
  get_link_properties_client_;
  std::shared_ptr<rclcpp::Client<gazebo_msgs::srv::GetLightProperties>>
  get_light_properties_client_;
  std::shared_ptr<rclcpp::Client<gazebo_msgs::srv::SetJointProperties>>
  set_joint_properties_client_;
  std::shared_ptr<rclcpp::Client<gazebo_msgs::srv::SetLinkProperties>>
  set_link_properties_client_;
  std::shared_ptr<rclcpp::Client<gazebo_msgs::srv::SetLightProperties>>
  set_light_properties_client_;
};

void GazeboRosPropertiesTest::SetUp()
{
  // Load world with state plugin and start paused
  this->Load("worlds/gazebo_ros_properties_test.world", true);

  // World
  world_ = gazebo::physics::get_world();
  ASSERT_NE(nullptr, world_);

  // Create ROS clients
  node_ = std::make_shared<rclcpp::Node>("gazebo_ros_properties_test");
  ASSERT_NE(nullptr, node_);

  get_model_properties_client_ =
    node_->create_client<gazebo_msgs::srv::GetModelProperties>("test/get_model_properties");
  ASSERT_NE(nullptr, get_model_properties_client_);
  EXPECT_TRUE(get_model_properties_client_->wait_for_service(std::chrono::seconds(1)));

  get_joint_properties_client_ =
    node_->create_client<gazebo_msgs::srv::GetJointProperties>("test/get_joint_properties");
  ASSERT_NE(nullptr, get_joint_properties_client_);
  EXPECT_TRUE(get_joint_properties_client_->wait_for_service(std::chrono::seconds(1)));

  get_link_properties_client_ =
    node_->create_client<gazebo_msgs::srv::GetLinkProperties>("test/get_link_properties");
  ASSERT_NE(nullptr, get_link_properties_client_);
  EXPECT_TRUE(get_link_properties_client_->wait_for_service(std::chrono::seconds(1)));

  get_light_properties_client_ =
    node_->create_client<gazebo_msgs::srv::GetLightProperties>("test/get_light_properties");
  ASSERT_NE(nullptr, get_light_properties_client_);
  EXPECT_TRUE(get_light_properties_client_->wait_for_service(std::chrono::seconds(1)));

  set_joint_properties_client_ =
    node_->create_client<gazebo_msgs::srv::SetJointProperties>("test/set_joint_properties");
  ASSERT_NE(nullptr, set_joint_properties_client_);
  EXPECT_TRUE(set_joint_properties_client_->wait_for_service(std::chrono::seconds(1)));

  set_link_properties_client_ =
    node_->create_client<gazebo_msgs::srv::SetLinkProperties>("test/set_link_properties");
  ASSERT_NE(nullptr, set_link_properties_client_);
  EXPECT_TRUE(set_link_properties_client_->wait_for_service(std::chrono::seconds(1)));

  set_light_properties_client_ =
    node_->create_client<gazebo_msgs::srv::SetLightProperties>("test/set_light_properties");
  ASSERT_NE(nullptr, set_light_properties_client_);
  EXPECT_TRUE(set_light_properties_client_->wait_for_service(std::chrono::seconds(1)));
}

void GazeboRosPropertiesTest::GetModelProperties(
  const std::string & _model_name)
{
  // Get spawned Model properties
  auto entity = world_->EntityByName(_model_name);
  ASSERT_NE(nullptr, entity);

  auto request = std::make_shared<gazebo_msgs::srv::GetModelProperties::Request>();
  request->model_name = _model_name;

  auto response_future = get_model_properties_client_->async_send_request(request);
  EXPECT_EQ(rclcpp::executor::FutureReturnCode::SUCCESS,
    rclcpp::spin_until_future_complete(node_, response_future));

  auto response = response_future.get();
  ASSERT_NE(nullptr, response);
  EXPECT_TRUE(response->success);

  // gazebo models simple_arm
  EXPECT_EQ(response->parent_model_name, "");
  EXPECT_EQ(response->canonical_link_name, "simple_arm::arm_base");

  EXPECT_EQ(response->link_names[0], "simple_arm::arm_base");
  EXPECT_EQ(response->link_names[1], "simple_arm::arm_shoulder_pan");
  EXPECT_EQ(response->link_names[2], "simple_arm::arm_elbow_pan");
  EXPECT_EQ(response->link_names[3], "simple_arm::arm_wrist_lift");
  EXPECT_EQ(response->link_names[4], "simple_arm::arm_wrist_roll");

  EXPECT_EQ(response->collision_names[0], "arm_base_collision");
  EXPECT_EQ(response->collision_names[1], "arm_base_collision_arm_trunk");
  EXPECT_EQ(response->collision_names[2], "arm_shoulder_pan_collision");
  EXPECT_EQ(response->collision_names[3], "arm_shoulder_pan_collision_arm_shoulder");
  EXPECT_EQ(response->collision_names[4], "arm_elbow_pan_collision");
  EXPECT_EQ(response->collision_names[5], "arm_elbow_pan_collision_arm_elbow");
  EXPECT_EQ(response->collision_names[6], "arm_elbow_pan_collision_arm_wrist");
  EXPECT_EQ(response->collision_names[7], "arm_wrist_lift_collision");
  EXPECT_EQ(response->collision_names[8], "arm_wrist_roll_collision");

  EXPECT_EQ(response->joint_names[0], "arm_shoulder_pan_joint");
  EXPECT_EQ(response->joint_names[1], "arm_elbow_pan_joint");
  EXPECT_EQ(response->joint_names[2], "arm_wrist_lift_joint");

  std::vector<std::string> v;  // Empty
  EXPECT_EQ(response->child_model_names, v);
  EXPECT_FALSE(response->is_static);
}

void GazeboRosPropertiesTest::GetJointProperties(
  const std::string & _joint_name,
  const double & _damping)
{
  auto request = std::make_shared<gazebo_msgs::srv::GetJointProperties::Request>();
  request->joint_name = _joint_name;

  auto response_future = get_joint_properties_client_->async_send_request(request);
  EXPECT_EQ(rclcpp::executor::FutureReturnCode::SUCCESS,
    rclcpp::spin_until_future_complete(node_, response_future));

  auto response = response_future.get();
  ASSERT_NE(nullptr, response);
  EXPECT_TRUE(response->success);

  std::vector<double> v;
  v.push_back(_damping);
  if ((response->damping).size() > 0) {
    EXPECT_EQ(response->damping, v);
  }
}

void GazeboRosPropertiesTest::SetJointProperties(
  const std::string & _joint_name,
  const double & _damping)
{
  auto request = std::make_shared<gazebo_msgs::srv::SetJointProperties::Request>();
  request->joint_name = _joint_name;

  std::vector<double> v;
  v.push_back(_damping);
  request->ode_joint_config.damping = v;

  auto response_future = set_joint_properties_client_->async_send_request(request);
  EXPECT_EQ(rclcpp::executor::FutureReturnCode::SUCCESS,
    rclcpp::spin_until_future_complete(node_, response_future));

  auto response = response_future.get();
  ASSERT_NE(nullptr, response);
  EXPECT_TRUE(response->success);
}

void GazeboRosPropertiesTest::GetLinkProperties(
  const std::string & _link_name,
  const bool & _gravity_mode,
  const double & _mass,
  const double & _ixx,
  const double & _ixy,
  const double & _ixz,
  const double & _iyy,
  const double & _iyz,
  const double & _izz)
{
  auto request = std::make_shared<gazebo_msgs::srv::GetLinkProperties::Request>();
  request->link_name = _link_name;

  auto response_future = get_link_properties_client_->async_send_request(request);
  EXPECT_EQ(rclcpp::executor::FutureReturnCode::SUCCESS,
    rclcpp::spin_until_future_complete(node_, response_future));

  auto response = response_future.get();
  ASSERT_NE(nullptr, response);
  EXPECT_TRUE(response->success);

  EXPECT_EQ(_gravity_mode, response->gravity_mode) << _link_name;
  EXPECT_EQ(_mass, response->mass) << _link_name;
  EXPECT_EQ(_ixx, response->ixx) << _link_name;
  EXPECT_EQ(_ixy, response->ixy) << _link_name;
  EXPECT_EQ(_ixz, response->ixz) << _link_name;
  EXPECT_EQ(_iyy, response->iyy) << _link_name;
  EXPECT_EQ(_iyz, response->iyz) << _link_name;
  EXPECT_EQ(_izz, response->izz) << _link_name;
}

void GazeboRosPropertiesTest::SetLinkProperties(
  const std::string & _link_name,
  const bool & _gravity_mode,
  const double & _mass,
  const double & _ixx,
  const double & _ixy,
  const double & _ixz,
  const double & _iyy,
  const double & _iyz,
  const double & _izz)
{
  auto request = std::make_shared<gazebo_msgs::srv::SetLinkProperties::Request>();
  request->link_name = _link_name;
  request->gravity_mode = _gravity_mode;
  request->mass = _mass;
  request->ixx = _ixx;
  request->ixy = _ixy;
  request->ixz = _ixz;
  request->iyy = _iyy;
  request->iyz = _iyz;
  request->izz = _izz;

  auto response_future = set_link_properties_client_->async_send_request(request);
  EXPECT_EQ(rclcpp::executor::FutureReturnCode::SUCCESS,
    rclcpp::spin_until_future_complete(node_, response_future));

  auto response = response_future.get();
  ASSERT_NE(nullptr, response);
  EXPECT_TRUE(response->success);
}

void GazeboRosPropertiesTest::GetLightProperties(
  const std::string & _light_name,
  const ignition::math::Vector4d & _diffuse,
  const double & _attenuation_constant,
  const double & _attenuation_linear,
  const double & _attenuation_quadratic)
{
  auto request = std::make_shared<gazebo_msgs::srv::GetLightProperties::Request>();
  request->light_name = _light_name;

  auto response_future = get_light_properties_client_->async_send_request(request);
  EXPECT_EQ(rclcpp::executor::FutureReturnCode::SUCCESS,
    rclcpp::spin_until_future_complete(node_, response_future));

  auto response = response_future.get();
  ASSERT_NE(nullptr, response);
  EXPECT_TRUE(response->success);

  EXPECT_NEAR(_diffuse.X(), response->diffuse.r, tol) << _light_name;
  EXPECT_NEAR(_diffuse.Y(), response->diffuse.g, tol) << _light_name;
  EXPECT_NEAR(_diffuse.Z(), response->diffuse.b, tol) << _light_name;
  EXPECT_NEAR(_diffuse.W(), response->diffuse.a, tol) << _light_name;

  EXPECT_NEAR(_attenuation_constant, response->attenuation_constant, tol) << _light_name;
  EXPECT_NEAR(_attenuation_linear, response->attenuation_linear, tol) << _light_name;
  EXPECT_NEAR(_attenuation_quadratic, response->attenuation_quadratic, tol) << _light_name;
}

void GazeboRosPropertiesTest::SetLightProperties(
  const std::string & _light_name,
  const ignition::math::Vector4d & _diffuse,
  const double & _attenuation_constant,
  const double & _attenuation_linear,
  const double & _attenuation_quadratic)
{
  auto request = std::make_shared<gazebo_msgs::srv::SetLightProperties::Request>();
  request->light_name = _light_name;
  request->diffuse.r = _diffuse.X();
  request->diffuse.g = _diffuse.Y();
  request->diffuse.b = _diffuse.Z();
  request->diffuse.a = _diffuse.W();
  request->attenuation_constant = _attenuation_constant;
  request->attenuation_linear = _attenuation_linear;
  request->attenuation_quadratic = _attenuation_quadratic;

  auto response_future = set_light_properties_client_->async_send_request(request);
  EXPECT_EQ(rclcpp::executor::FutureReturnCode::SUCCESS,
    rclcpp::spin_until_future_complete(node_, response_future));

  auto response = response_future.get();
  ASSERT_NE(nullptr, response);
  EXPECT_TRUE(response->success);
}

TEST_F(GazeboRosPropertiesTest, GetSetProperties)
{
  // Get model properties
  {
    this->GetModelProperties("simple_arm");
  }
  // Get / set joint propertires
  {
    // Get initial joint properties (damping only).
    // Note that damping=1.0 is the default value for this particular joint.
    this->GetJointProperties("simple_arm::arm_shoulder_pan_joint", 1.0);

    // Set joint properties (damping only)
    this->SetJointProperties("simple_arm::arm_shoulder_pan_joint", 0.5);

    // Check new joint properties (damping only).
    this->GetJointProperties("simple_arm::arm_shoulder_pan_joint", 0.5);
  }

  // Get / set link properties
  {
    // Get initial link properties
    this->GetLinkProperties("simple_arm::arm_base",
      true, 101.0, 1.11, 0.0, 0.0, 100.11, 0.0, 1.01);

    // Set link properties
    this->SetLinkProperties("simple_arm::arm_base",
      true, 170.2, 1.2, 0.3, 0.2, 102.2, 0.2, 1.02);

    // Check new link properties
    this->GetLinkProperties("simple_arm::arm_base",
      true, 170.2, 1.2, 0.3, 0.2, 102.2, 0.2, 1.02);
  }
  // Get / set light properties
  {
    // Get initial light properties
    this->GetLightProperties("sun", ignition::math::Vector4d(0.800000011920929, 0.800000011920929,
      0.800000011920929, 1.0), 0.8999999761581421, 0.009999999776482582, 0.0010000000474974513);

    // Set light properties
    this->SetLightProperties("sun", ignition::math::Vector4d(0.7, 0.1, 0.5, 1.0),
      0.92, 0.0092, 0.002);

    // Check new light properties. Wait for properties to be set first.
    rclcpp::sleep_for(std::chrono::milliseconds(500));
    this->GetLightProperties("sun", ignition::math::Vector4d(0.7, 0.1, 0.5, 1.0),
      0.92, 0.0092, 0.002);
  }
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
