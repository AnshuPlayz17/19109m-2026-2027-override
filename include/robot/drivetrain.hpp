#pragma once

#include "lemlib/api.hpp"
#include "pros/distance.hpp"
#include "pros/imu.hpp"
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"

/**
 * The drivetrain and the sensors that feed odometry.
 *
 * These are defined once in drivetrain.cpp and declared here so autonomous
 * routines and driver control share the same objects.
 */
namespace robot {

extern pros::MotorGroup leftDrive;
extern pros::MotorGroup rightDrive;

extern pros::Imu imu;
extern pros::Rotation verticalTracker;
extern pros::Rotation horizontalTracker;
extern pros::Distance distanceSensor;

extern lemlib::Chassis chassis;

/**
 * Calibrates the IMU and zeroes the tracking wheels.
 *
 * Blocks until calibration finishes, so call it from initialize() and keep the
 * robot still while it runs.
 */
void initDrivetrain();

}  // namespace robot
