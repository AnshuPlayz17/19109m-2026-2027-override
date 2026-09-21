#include "main.h"

#include "pros/llemu.hpp"
#include "pros/misc.hpp"
#include "robot/drivetrain.hpp"
#include "robot/wall_reset.hpp"

namespace {

// Hold this to snap the odometry pose against a wall. Useful for checking the
// distance sensor offset during testing; safe to remove once it is tuned.
constexpr pros::controller_digital_e_t kWallResetButton = pros::E_CONTROLLER_DIGITAL_Y;

}  // namespace

void initialize() {
    pros::lcd::initialize();

    // Blocks while the IMU calibrates -- keep the robot still.
    robot::initDrivetrain();

    // Live pose readout, which is the fastest way to spot bad odometry
    // constants before they cost an autonomous run.
    pros::Task screenTask([]() {
        while (true) {
            const lemlib::Pose pose = robot::chassis.getPose();
            pros::lcd::print(0, "19109M | Override");
            pros::lcd::print(1, "X: %.2f", pose.x);
            pros::lcd::print(2, "Y: %.2f", pose.y);
            pros::lcd::print(3, "Theta: %.2f", pose.theta);
            pros::delay(50);
        }
    });
}

void disabled() {}

void competition_initialize() {}

void autonomous() {
    // Field position at the start of the run. Set this to wherever the robot
    // is actually placed before each routine.
    robot::chassis.setPose(0, 0, 0);

    // Routines go here. LemLib's motions block until they finish or time out:
    //
    //   robot::chassis.moveToPoint(0, 24, 2000);
    //   robot::chassis.turnToHeading(90, 1000);
    //   robot::chassis.moveToPose(24, 24, 90, 3000);
    //
    // After driving into a wall and squaring up, correct the drift:
    //
    //   robot::resetFromWall();
}

void opcontrol() {
    pros::Controller master(pros::E_CONTROLLER_MASTER);

    while (true) {
        // Split arcade: left stick drives, right stick turns.
        const int throttle = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        const int turn = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        robot::chassis.arcade(throttle, turn);

        if (master.get_digital_new_press(kWallResetButton)) {
            master.rumble(robot::resetFromWall() ? "." : "--");
        }

        pros::delay(20);
    }
}
