#include "robot/drivetrain.hpp"

#include "robot/config.hpp"

namespace robot {

using namespace config;

pros::MotorGroup leftDrive(kLeftDrivePorts, pros::MotorGearset::blue);
pros::MotorGroup rightDrive(kRightDrivePorts, pros::MotorGearset::blue);

pros::Imu imu(kImuPort);
pros::Rotation verticalTracker(kVerticalTrackerPort);
pros::Rotation horizontalTracker(kHorizontalTrackerPort);
pros::Distance distanceSensor(kDistanceSensorPort);

namespace {

lemlib::TrackingWheel verticalWheel(&verticalTracker, kVerticalTrackerDiameter,
                                    kVerticalTrackerOffset);
lemlib::TrackingWheel horizontalWheel(&horizontalTracker, kHorizontalTrackerDiameter,
                                      kHorizontalTrackerOffset);

lemlib::Drivetrain drivetrain(&leftDrive, &rightDrive, kTrackWidth, kDriveWheelDiameter, kDriveRpm,
                              kHorizontalDrift);

// TODO(19109M): these are LemLib's starting gains, not tuned values. Tune the
// lateral controller first (straight moves), then the angular one (turns).
// Arguments: kP, kI, kD, anti-windup, small error, small timeout, large error,
// large timeout, slew.
lemlib::ControllerSettings lateralController(10, 0, 3, 3, 1, 100, 3, 500, 20);
lemlib::ControllerSettings angularController(2, 0, 10, 3, 1, 100, 3, 500, 0);

// Two tracking wheels: one vertical, one horizontal, with the IMU for heading.
// The unused slots are the second vertical/horizontal wheels we don't have.
lemlib::OdomSensors sensors(&verticalWheel, nullptr, &horizontalWheel, nullptr, &imu);

}  // namespace

lemlib::Chassis chassis(drivetrain, lateralController, angularController, sensors);

void initDrivetrain() {
    // Blocks for roughly two seconds while the IMU calibrates. The robot must
    // stay still, or heading will be wrong for the whole match.
    chassis.calibrate();

    verticalTracker.reset_position();
    horizontalTracker.reset_position();
}

}  // namespace robot
