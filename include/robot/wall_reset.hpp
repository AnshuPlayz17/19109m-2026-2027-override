#pragma once

namespace robot {

/**
 * Corrects one axis of the odometry pose using the distance sensor.
 *
 * Tracking-wheel odometry drifts over a match. When the robot is square to a
 * field wall, the distance sensor gives an absolute reading that pins down the
 * axis perpendicular to that wall, so we overwrite just that coordinate and
 * leave the other one (and the heading) to odometry.
 *
 * Preconditions, all of which are checked:
 *   - the robot is within kMaxHeadingError of square to a wall
 *   - the sensor reports a usable reading inside kMaxWallDistance
 *
 * @return true if the pose was corrected, false if the reading was rejected.
 *         A false return means the pose is unchanged, not that the robot is
 *         lost -- callers should treat it as "try again somewhere better".
 */
bool resetFromWall();

// A VEX field is 144" square; with LemLib's centered coordinates the walls sit
// at +/-72 on each axis.
inline constexpr float kFieldHalfWidth = 72.0f;

// Reject a reset if the robot is more than this far from square to the wall,
// since the reading no longer maps cleanly onto one axis.
inline constexpr float kMaxHeadingError = 7.0f;

// Distance sensor accuracy falls off with range; ignore far readings.
inline constexpr float kMaxWallDistance = 40.0f;

}  // namespace robot
