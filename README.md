# 19109M — VEX V5 Override (2026–2027)

Driver control for team 19109M, on [PROS 4](https://pros.cs.purdue.edu/).
Split arcade, drive only — no odometry, no autonomous, no external libraries.

## Fastest path to driving

If you already have a PROS project, just replace its `src/main.cpp` with the
one from this repo and upload. That file is self-contained.

Otherwise:

1. Install the **PROS** extension in VS Code. It bundles the CLI and the ARM
   toolchain, so nothing else needs installing.
2. Command palette → `PROS: Create Project`. Pick a folder, kernel `4.x`, no
   extra templates.
3. Replace the generated `src/main.cpp` with this repo's `src/main.cpp`.
4. Hit **Build**, then **Upload**.

To build this repo directly instead, open the **PROS integrated terminal** and
run `./scripts/bootstrap.sh` once to generate the build skeleton, then use the
Build and Upload buttons.

```bash
pros make                  # build
pros upload --slot 1       # flash to the brain
pros terminal              # view printf output
```

## Controls

| Input | Action |
| ----- | ------ |
| Left stick Y | Forward / back |
| Right stick X | Turn |

## Ports

| Port | Motor |
| ---- | ----- |
| 1 | Left front (reversed) |
| 2 | Left back (reversed) |
| 3 | Right front |
| 4 | Right back |

All four are set at the top of `src/main.cpp`. **Running a 6-motor drive?** Add
the third port to each list and nothing else changes:

```cpp
const std::vector<std::int8_t> kLeftPorts = {-1, -2, -3};
const std::vector<std::int8_t> kRightPorts = {3, 4, 5};
```

If the robot drives backwards, flip the signs on both lists.

## Tuning the feel

Three constants near the top of `src/main.cpp`:

- `kDeadzone` (20) — stick movement smaller than this counts as released, so
  the robot doesn't creep. Raise it if the robot drifts on its own.
- `kTurnScale` (0.7) — how much of the right stick becomes turning. Lower is
  calmer, `1.0` is full twitchy.
- Brake mode is set in `initialize()`. Swap `E_MOTOR_BRAKE_BRAKE` for
  `E_MOTOR_BRAKE_COAST` if you'd rather the robot roll when you let go.

## Checks

```bash
./scripts/syntax-check.sh    # type-check against the real PROS headers
```

Clones the PROS kernel headers and parses `src/` with a host `g++`. No ARM
toolchain needed, so CI runs it on every push. It does **not** link or produce a
binary — only `pros make` does that.

## What happened to the odometry code

An earlier version had a LemLib chassis, two-tracking-wheel odometry and a
distance-sensor wall reset. It was removed to get driving with the fewest
moving parts. It's in git history — see the commit before this one, or
[#1](https://github.com/AnshuPlayz17/19109m-2026-2027-override/pull/1).
