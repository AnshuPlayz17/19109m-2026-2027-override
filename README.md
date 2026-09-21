# 19109M — VEX V5 Override (2026–2027)

Competition code for team 19109M, built on [PROS 4](https://pros.cs.purdue.edu/)
with [LemLib](https://lemlib.readthedocs.io/) for odometry and motion control.

## Setup (VS Code)

The PROS build skeleton and the LemLib template are generated rather than
committed, so a fresh clone needs one bootstrap step before it will build.

1. Install the **PROS** extension in VS Code. It bundles the PROS CLI and the
   ARM toolchain, so nothing else needs installing.
2. Clone and open the folder:
   ```bash
   git clone https://github.com/AnshuPlayz17/19109m-2026-2027-override.git
   ```
3. Open the **PROS integrated terminal** (command palette → `PROS: Integrated
   Terminal`) and run:
   ```bash
   ./scripts/bootstrap.sh
   ```
   A normal system terminal will not work unless you installed the CLI
   separately — the extension only puts `pros` on the PATH of its own terminal.
4. Reload the window. The PROS sidebar now shows Build and Upload.

Bootstrap generates `Makefile`, `common.mk`, `firmware/` and `include/pros/`,
registers the LemLib depot and installs LemLib. It never overwrites files
already in the repo, so your code always wins over the generated defaults.

Equivalent commands, if you prefer the terminal:

```bash
pros make                  # build
pros upload --slot 1       # flash to the brain
pros terminal              # view printf output
```

### First upload checklist

- Motors plugged into **1, 2** (left front, left back) and **3, 4** (right
  front, right back). A drive motor on the wrong port means that side will not
  move.
- If the robot drives backwards, flip the signs on both port lists in
  `include/robot/config.hpp`.
- The IMU on port 10 is not needed to drive. If it is missing, LemLib retries
  calibration five times, disables it, and continues to driver control — you
  lose odometry and wait a few extra seconds at startup, but the robot drives.

## Robot

Four 11W motors, blue (600 rpm) cartridges, 36:48 external gearing, 3.25" omnis
on all four corners — **450 rpm at the wheels**. Driver control is split arcade.

### Port map

| Port | Device | Notes |
| ---- | ------ | ----- |
| 1 | Left front drive | reversed in `config.hpp` |
| 2 | Left back drive | reversed in `config.hpp` |
| 3 | Right front drive | |
| 4 | Right back drive | |
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

## Checks

Two things run in CI and both run locally.

```bash
./scripts/syntax-check.sh    # type-check against real PROS + LemLib headers
find src include \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) -exec clang-format -i {} +
```

`syntax-check.sh` clones the LemLib headers (which vendor PROS) and parses the
sources with a host `g++`. It needs no ARM toolchain, so it catches renamed
methods and wrong constructor arguments on every push. It does **not** link or
produce a binary — only `pros make` does that.
