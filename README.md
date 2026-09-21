# 19109M — VEX V5 Override (2026–2027)

Competition code for team 19109M, built on [PROS 4](https://pros.cs.purdue.edu/)
with [LemLib](https://lemlib.readthedocs.io/) for odometry and motion control.

## Setup

The PROS build skeleton and the LemLib template are generated rather than
committed, so a fresh clone needs one bootstrap step:

```bash
git clone https://github.com/AnshuPlayz17/19109m-2026-2027-override.git
cd 19109m-2026-2027-override
./scripts/bootstrap.sh
```

That generates `Makefile`, `common.mk`, `firmware/` and `include/pros/`, then
installs LemLib. It never overwrites files already in the repo.

```bash
pros make                  # build
pros upload --slot 1       # flash to the brain
pros terminal              # view printf output
```

## Robot

Four 11W motors, blue (600 rpm) cartridges, 36:48 external gearing, 3.25" omnis
on all four corners — **450 rpm at the wheels**. Driver control is split arcade.

### Port map

| Port | Device | Notes |
| ---- | ------ | ----- |
| 1, 2 | Left drive | reversed in `config.hpp` |
| 3, 4 | Right drive | |
| 10 | IMU | heading |
| 11 | Rotation sensor | vertical tracking wheel |
| 12 | Rotation sensor | horizontal tracking wheel |
| 13 | Distance sensor | wall resets |

Every port lives in `include/robot/config.hpp`. Change them there, not in the
subsystem code.

## Layout

```
include/robot/config.hpp       ports, geometry, sensor offsets
include/robot/drivetrain.hpp   chassis + odometry sensors
include/robot/wall_reset.hpp   distance sensor pose correction
src/main.cpp                   competition entry points
src/robot/                     implementations
```

## Before the robot drives well

Four constants are measured off the real robot and are currently placeholders.
Odometry will be wrong until they are set — all are marked `TODO(19109M)` in
`include/robot/config.hpp`:

- `kTrackWidth` — center-to-center between the left and right wheel contact patches
- `kVerticalTrackerOffset` — signed distance from tracking center, positive to the right
- `kHorizontalTrackerOffset` — signed distance from tracking center, positive forward
- `kDistanceSensorOffset` — sensor face back to tracking center

The PID gains in `src/robot/drivetrain.cpp` are LemLib's defaults, not tuned
values. Tune the lateral controller on straight moves first, then the angular
one on turns.

## Odometry and wall resets

Position comes from the two tracking wheels; heading comes from the IMU.

The distance sensor is **not** part of LemLib's odometry model — it is used
separately. `robot::resetFromWall()` reads the wall when the robot is square to
one and overwrites just that axis of the pose, leaving the other axis and the
heading alone. It refuses to act on a skewed angle, a low-confidence reading, or
a target too far away, and returns `false` in those cases.

Holding **Y** in driver control triggers a reset: a short rumble means it
applied, a long one means the reading was rejected. That binding is there to
check `kDistanceSensorOffset` on a real field and can be removed once tuned.

## CI

GitHub Actions checks formatting with `clang-format`. Run it yourself with:

```bash
find src include \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) -exec clang-format -i {} +
```

A compile job is scaffolded but commented out in `.github/workflows/ci.yml` —
it needs an ARM toolchain step that works before it is worth enabling.
