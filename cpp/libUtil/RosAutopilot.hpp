/// Node to receive autopilot data

#include "std_msgs/String.h"
#include <fdnn/concurrency/DataDeque.hpp>
#include <gt_uav_msgs/Attitude.h>
#include <gt_uav_msgs/SimpleUTMPose.h>
#include <ros/ros.h>

#include <memory>

class RosAutopilot : public fdnn::Autopilot {
public:
	RosAutopilot(ros::NodeHandle &node)
      : fdnn::Autopilot(fdnn::Config()) {
    utm_sub_ = node.subscribe<gt_uav_msgs::SimpleUTMPose>(
        "/poseOut", 10,
        std::bind(&RosAutopilot::callback, this, std::placeholders::_1));

    atti_sub_ = node.subscribe<gt_uav_msgs::Attitude>(
        "/Attitude", 10,
        std::bind(&RosAutopilot::attitude_callback, this,
                  std::placeholders::_1));
  }

  virtual void connect() {}

  virtual void disconnect() {}

  void callback(const gt_uav_msgs::SimpleUTMPose::ConstPtr msg) {
    fdnn::AutopilotState state;
    state.timestamp = msg->rostime * 1000;
    state.attitude = attitude_.getStateNear(state.timestamp);
    state.gps_tag.lat = msg->lat;
    state.gps_tag.lon = msg->lon;

    state.gps_tag.relative_alt = msg->haglalt;
    state.gps_tag.alt = msg->baroalt;
    state.est_agl_m = state.gps_tag.relative_alt;
    state.gps_tag.timestamp = state.timestamp;

    // jreid guesses at other components
    state.gps_tag.velocity = {msg->vx, msg->vy, msg->vz};

    //std::cout << std::fixed << std::setprecision(6) << "AP " << msg->lat << " " << msg->lon << " " << state.gps_tag.relative_alt << std::endl;

    state_src_.broadcast(state);
  }

  void attitude_callback(const gt_uav_msgs::Attitude::ConstPtr msg) {
    fdnn::Attitude attitude;
    attitude.timestamp = msg->rostime * 1000;
    attitude.roll = msg->roll;
    attitude.pitch = msg->pitch;
    attitude.yaw = msg->yaw;
    attitude.roll_rate = msg->rollspeed;
    attitude.pitch_rate = msg->pitchspeed;
    attitude.yaw_rate = msg->yawspeed;
    attitude_.addNewState(attitude);
  }

  virtual bool startMission() {}
  virtual bool resumeMission() {}
  virtual void stopMission() {}
  virtual void setServo(int servo_num, float percentage) {}

  virtual void startExternalControl() {}

  virtual void
  setTranslateGoal(cv::Point3f goal_meters,
                   const fdnn::DescentInfo &descent_info) {}

  virtual void
  updateTargetFollow(const fdnn::ObjectPose &target,
                     const fdnn::PlatformInfo &platform,
                     fdnn::YawInfo desired_yaw,
                     const fdnn::DescentInfo &descent_info) {}

private:
  ros::Subscriber utm_sub_;
  ros::Subscriber atti_sub_;

  fdnn::DataDeque<fdnn::Attitude> attitude_;
};
