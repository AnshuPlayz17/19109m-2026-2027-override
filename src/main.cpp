#include "main.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <vector>

// 19109M -- Override (2026-2027)
// Driver control only. Split arcade: left stick drives, right stick turns.

// --- Ports ------------------------------------------------------------------
// Negative means the motor is reversed. The left side is reversed by
// convention; if the robot drives backwards, flip the signs on BOTH sides.
//
//   1 = left front     3 = right front
//   2 = left back      4 = right back
//
// Running a 6-motor drive? Add the third port to each list below and nothing
// else has to change.
const std::vector<std::int8_t> kLeftPorts = {-1, -2};
const std::vector<std::int8_t> kRightPorts = {3, 4};

// --- Feel -------------------------------------------------------------------
// Sticks rarely rest at exactly zero. Anything smaller than this counts as
// released, so the robot doesn't creep on its own.
constexpr int kDeadzone = 20;

// Turning at full stick is twitchy on a fast drive. Scale it down for control.
constexpr double kTurnScale = 0.7;

// The V5 motor command range.
constexpr int kMaxPower = 127;

pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::MotorGroup driveLeft(kLeftPorts);
pros::MotorGroup driveRight(kRightPorts);

namespace {

/** Treats small stick values as zero. */
int applyDeadzone(int value) {
    return std::abs(value) < kDeadzone ? 0 : value;
}

}  // namespace

void initialize() {
    // Hold position when the sticks are released, instead of rolling on.
    driveLeft.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    driveRight.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

void opcontrol() {
    while (true) {
        const int forward = applyDeadzone(master.get_analog(ANALOG_LEFT_Y));
        const int turn =
            static_cast<int>(applyDeadzone(master.get_analog(ANALOG_RIGHT_X)) * kTurnScale);

        // Arcade mixing. Clamping matters: forward + turn can reach 254, and
        // handing a motor more than 127 would otherwise wrap or saturate badly.
        driveLeft.move(std::clamp(forward + turn, -kMaxPower, kMaxPower));
        driveRight.move(std::clamp(forward - turn, -kMaxPower, kMaxPower));

        pros::delay(20);
    }
}
