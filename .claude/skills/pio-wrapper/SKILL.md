---
name: pio-wrapper
description: Run every PlatformIO (pio) command through this wrapper. It prints a short summary (result, memory use, deduplicated errors and warnings with source context) instead of the full build log, and saves the full log to .pio/pio-wrapper.log.
---

# PlatformIO wrapper

Run `pio` only through this wrapper. Never run `pio` directly: a full build log costs thousands of tokens.

## Usage

Run from the repository root. Pass the normal `pio` arguments:

```bash
python3 .claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble
python3 .claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t clean
python3 .claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble -t upload
```

Add `--all-warnings` to list each warning from libraries and the framework instead of a count for each file.

`pio device monitor` is refused, because it does not end by itself. Ask the user to run it.

## Output

```text
✓ pio run -e use_nimble succeeded in 41s
Warnings (project): 1
src/Menu/Model/MenuTree.h:12:10: warning: unused variable 'x' [-Wunused-variable]  (x3)
   12 |     int x;
      |         ^
Warnings (dependencies): 37 in 4 files
   20  .pio/libdeps/use_nimble/NimBLE-Arduino/src/NimBLEDevice.cpp
  Add --all-warnings to list them.
RAM:   [=         ]  12.3% (used 40312 bytes from 327680 bytes)
Flash: [=======   ]  70.1% (used 1234567 bytes from 1310720 bytes)
Full log: .pio/pio-wrapper.log
```

- The first line gives the result, the exit code on failure, and the time.
- `Errors` and `Warnings (project)` show each compiler diagnostic once, with its include context and source lines. `(xN)` shows how often it repeated.
- `Warnings (dependencies)` counts the warnings in `.pio/libdeps/` and in the PlatformIO packages for each file.
- The output has a maximum of 120 lines. For more detail, `grep` the full log, for example `grep -n "warning:" .pio/pio-wrapper.log`.
- The exit code is the exit code of `pio`.
- A build is incremental: it shows only the warnings of the files that it compiles. To see all warnings, run `run -t clean` first.
