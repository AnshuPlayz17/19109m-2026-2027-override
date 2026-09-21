#pragma once

#include <cstdint>
#include <vector>

#include "lemlib/api.hpp"
#include "pros/adi.hpp"
#include "pros/distance.hpp"
#include "pros/imu.hpp"
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"

/**
 * Single source of truth for wiring and physical measurements.
 *
 * Everything the robot's geometry or wiring can change is a constant here, so
 * retuning after a rebuild means editing this file and nothing else.
 */
namespace robot::config {

// --- Ports ------------------------------------------------------------------
// Negative port numbers mean the motor is reversed. The left side is reversed
// by convention; if the robot drives backwards, flip the signs on both sides.
//
//   1 = left front    3 = right front
//   2 = left back     4 = right back
inline const std::vector<std::int8_t> kLeftDrivePorts = {-1, -2};
inline const std::vector<std::int8_t> kRightDrivePorts = {3, 4};

inline constexpr std::int8_t kImuPort = 10;
inline constexpr std::int8_t kVerticalTrackerPort = 11;
inline constexpr std::int8_t kHorizontalTrackerPort = 12;
inline constexpr std::int8_t kDistanceSensorPort = 13;

// --- Drivetrain geometry ----------------------------------------------------
// 4x 11W motors, blue (600 rpm) cartridges, 36:48 external gearing.
//   600 rpm * (36 / 48) = 450 rpm at the wheels.
inline constexpr float kDriveRpm = 450.0f;

// All four drive wheels are 3.25" omnis.
inline constexpr float kDriveWheelDiameter = lemlib::Omniwheel::NEW_325;

// Horizontal drift: 2 for an all-omni drive, 8 when running traction wheels.
inline constexpr int kHorizontalDrift = 2;

// TODO(19109M): measure on the real robot.
// Center-to-center distance between the left and right wheel contact patches.
inline constexpr float kTrackWidth = 11.0f;

// --- Tracking wheels --------------------------------------------------------
// TODO(19109M): measure on the real robot.
// Offsets are signed distances from the robot's tracking center:
//   vertical   -> positive when the wheel sits to the RIGHT of center
//   horizontal -> positive when the wheel sits FORWARD of center
inline constexpr float kVerticalTrackerDiameter = lemlib::Omniwheel::NEW_275;
inline constexpr float kVerticalTrackerOffset = -2.5f;

inline constexpr float kHorizontalTrackerDiameter = lemlib::Omniwheel::NEW_275;
inline constexpr float kHorizontalTrackerOffset = -5.75f;

// --- Distance sensor (wall resets) ------------------------------------------
// TODO(19109M): measure on the real robot.
// Distance from the face of the sensor back to the robot's tracking center,
// along the direction the sensor points.
inline constexpr float kDistanceSensorOffset = 6.0f;

// The sensor is mounted facing this direction relative to the robot's front,
// in degrees clockwise. 0 = forward, 90 = right, 180 = rear, 270 = left.
inline constexpr float kDistanceSensorHeading = 0.0f;

}  // namespace robot::config
