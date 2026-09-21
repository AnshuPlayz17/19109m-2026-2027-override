#include "robot/wall_reset.hpp"

#include <cmath>

#include "robot/config.hpp"
#include "robot/drivetrain.hpp"

namespace robot {

namespace {

constexpr float kMmPerInch = 25.4f;

// The V5 distance sensor reports confidence from 0 to 63. Low-confidence
// readings are usually a glancing angle or a target out of range.
constexpr std::int32_t kMinConfidence = 50;

/** Wraps an angle in degrees into [0, 360). */
float normalize(float degrees) {
    float wrapped = std::fmod(degrees, 360.0f);
    if (wrapped < 0.0f) wrapped += 360.0f;
    return wrapped;
}

/** Signed difference a - b in degrees, wrapped into (-180, 180]. */
float angleError(float a, float b) {
    float diff = normalize(a - b);
    if (diff > 180.0f) diff -= 360.0f;
    return diff;
}

}  // namespace

bool resetFromWall() {
    const lemlib::Pose pose = chassis.getPose();

    // Which way the sensor itself points in field coordinates.
    const float sensorHeading = normalize(pose.theta + config::kDistanceSensorHeading);

    // Snap to the nearest wall-facing direction and bail if we are too skewed
    // for the reading to describe a single axis.
    const float cardinal = normalize(std::round(sensorHeading / 90.0f) * 90.0f);
    if (std::fabs(angleError(sensorHeading, cardinal)) > kMaxHeadingError) return false;

    if (distanceSensor.get_confidence() < kMinConfidence) return false;

    const std::int32_t millimeters = distanceSensor.get();
    if (millimeters <= 0) return false;

    const float reading = static_cast<float>(millimeters) / kMmPerInch;
    if (reading > kMaxWallDistance) return false;

    // Distance from the robot's tracking center to the wall.
    const float toWall = reading + config::kDistanceSensorOffset;

    float x = pose.x;
    float y = pose.y;

    // LemLib headings are degrees clockwise from the +Y axis, so 0 faces the
    // far wall and 90 faces the right-hand wall.
    if (cardinal == 0.0f) {
        y = kFieldHalfWidth - toWall;
    } else if (cardinal == 90.0f) {
        x = kFieldHalfWidth - toWall;
    } else if (cardinal == 180.0f) {
        y = -kFieldHalfWidth + toWall;
    } else {
        x = -kFieldHalfWidth + toWall;
    }

    // Heading comes from the IMU and is more trustworthy than this reading, so
    // only the one position axis is overwritten.
    chassis.setPose(x, y, pose.theta);
    return true;
}

}  // namespace robot
