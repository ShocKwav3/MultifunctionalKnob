#!/usr/bin/env python3
"""Run a PlatformIO command and print a short summary instead of the full log.

Prints: the result, the memory usage, compiler errors and warnings (with their
source context, deduplicated), and the path of the full log.
"""

import os
import re
import shutil
import subprocess
import sys
import time

LOG_PATH = os.path.join(".pio", "pio-wrapper.log")
MAX_OUTPUT_LINES = 120
MAX_BLOCK_LINES = 12
FALLBACK_TAIL_LINES = 15

ANSI = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")
DIAGNOSTIC = re.compile(
    r"^(?P<path>[^\s:][^:]*):\d+(?::\d+)?: (?P<kind>fatal error|error|warning|note): "
)
CONTEXT = re.compile(
    r"^In file included from |^\s+from |: In (member )?function |: In constructor |"
    r": In destructor |: In instantiation of |: At global scope|: In static member function "
)
LINKER_ERROR = re.compile(r"undefined reference to|multiple definition of|collect2: error")
OTHER_ERROR = re.compile(r"\berror\b|\bfailed\b|\bexception\b", re.IGNORECASE)
OTHER_WARNING = re.compile(r"^warning\b|\bWarning!", re.IGNORECASE)
MEMORY = re.compile(r"^(RAM|Flash):\s")
BUILD_NOISE = re.compile(r"^\*\*\* \[|^=+ |^\S+\s+(FAILED|SUCCESS|IGNORED)\s+\d")
DEPENDENCY_PATH = re.compile(r"\.pio/libdeps/|\.platformio/(packages|platforms)/")


def find_pio():
    pio = shutil.which("pio")
    if pio:
        return pio
    fallback = os.path.expanduser("~/.platformio/penv/bin/pio")
    return fallback if os.path.exists(fallback) else None


def is_continuation(line):
    return line.startswith((" ", "\t")) or line.lstrip().startswith("|")


def parse(lines):
    """Split the log into diagnostic blocks, memory lines and other error/warning lines."""
    blocks = []
    memory = []
    other_errors = []
    other_warnings = []
    pending_context = []
    current = None

    for line in lines:
        diagnostic = DIAGNOSTIC.match(line)

        is_follow_up = diagnostic and line[diagnostic.end():].startswith("  ")
        if diagnostic and (diagnostic.group("kind") == "note" or is_follow_up) and current:
            current["lines"].append(line)
            continue
        if current and not diagnostic and is_continuation(line) and not CONTEXT.search(line):
            current["lines"].append(line)
            continue
        current = None

        if diagnostic and diagnostic.group("kind") != "note":
            kind = "warning" if diagnostic.group("kind") == "warning" else "error"
            current = {
                "kind": kind,
                "path": diagnostic.group("path"),
                "lines": pending_context + [line],
                "diagnostic_index": len(pending_context),
            }
            blocks.append(current)
            pending_context = []
        elif CONTEXT.search(line):
            pending_context.append(line)
        else:
            pending_context = []
            if BUILD_NOISE.match(line):
                continue
            if MEMORY.match(line):
                memory.append(line)
            elif LINKER_ERROR.search(line) or OTHER_ERROR.search(line):
                other_errors.append(line)
            elif OTHER_WARNING.search(line):
                other_warnings.append(line)

    return blocks, memory, other_errors, other_warnings


def unique_blocks(blocks, kind):
    seen = {}
    for block in blocks:
        if block["kind"] != kind:
            continue
        key = "\n".join(block["lines"][block["diagnostic_index"]:])
        if key in seen:
            seen[key]["count"] += 1
        else:
            seen[key] = dict(block, count=1)
    return list(seen.values())


def unique_lines(lines):
    return list(dict.fromkeys(lines))


def format_block(block):
    lines = block["lines"][:MAX_BLOCK_LINES]
    if len(block["lines"]) > MAX_BLOCK_LINES:
        lines.append(f"  ... {len(block['lines']) - MAX_BLOCK_LINES} more lines in the block")
    index = block["diagnostic_index"]
    if block["count"] > 1 and index < len(lines):
        lines[index] = f"{lines[index]}  (x{block['count']})"
    return lines


def dependency_summary(blocks):
    counts = {}
    for block in blocks:
        counts[block["path"]] = counts.get(block["path"], 0) + block["count"]
    return [f"  {count:>3}  {path}" for path, count in sorted(counts.items(), key=lambda item: -item[1])]


def build_report(blocks, memory, other_errors, other_warnings, failed, show_all_warnings):
    report = []
    errors = unique_blocks(blocks, "error")
    warnings = unique_blocks(blocks, "warning")
    project_warnings = [b for b in warnings if not DEPENDENCY_PATH.search(b["path"])]
    dependency_warnings = [b for b in warnings if DEPENDENCY_PATH.search(b["path"])]

    shown_other_errors = unique_lines(other_errors) if failed else []
    if errors or shown_other_errors:
        report.append(f"Errors: {len(errors) + len(shown_other_errors)}")
        for block in errors:
            report.extend(format_block(block))
        report.extend(shown_other_errors)

    if project_warnings or other_warnings:
        report.append(f"Warnings (project): {len(project_warnings) + len(unique_lines(other_warnings))}")
        for block in project_warnings:
            report.extend(format_block(block))
        report.extend(unique_lines(other_warnings))

    if dependency_warnings:
        total = sum(b["count"] for b in dependency_warnings)
        report.append(f"Warnings (dependencies): {total} in {len({b['path'] for b in dependency_warnings})} files")
        if show_all_warnings:
            for block in dependency_warnings:
                report.extend(format_block(block))
        else:
            report.extend(dependency_summary(dependency_warnings))
            report.append("  Add --all-warnings to list them.")

    report.extend(memory)
    return report


def write_log(output):
    os.makedirs(os.path.dirname(LOG_PATH), exist_ok=True)
    with open(LOG_PATH, "w", encoding="utf-8") as log_file:
        log_file.write(output)


def main():
    args = sys.argv[1:]
    show_all_warnings = "--all-warnings" in args
    args = [a for a in args if a != "--all-warnings"]

    if "monitor" in args:
        print("✗ Not run: 'pio device monitor' does not end by itself. Ask the user to run it.")
        sys.exit(2)

    pio = find_pio()
    if not pio:
        print("✗ 'pio' not found on PATH or in ~/.platformio/penv/bin. Install PlatformIO first.")
        sys.exit(127)

    command = "pio " + " ".join(args)
    start = time.monotonic()
    try:
        result = subprocess.run([pio] + args, capture_output=True, text=True, check=False)
    except OSError as error:
        print(f"✗ Could not run {command}: {error}")
        sys.exit(1)
    elapsed = time.monotonic() - start

    output = ANSI.sub("", result.stdout + result.stderr)
    write_log(output)
    lines = output.splitlines()
    failed = result.returncode != 0

    blocks, memory, other_errors, other_warnings = parse(lines)
    report = build_report(blocks, memory, other_errors, other_warnings, failed, show_all_warnings)

    if failed and not report:
        report = ["Last lines of the log:"] + [l for l in lines if l.strip()][-FALLBACK_TAIL_LINES:]

    status = f"✗ {command} failed (exit {result.returncode})" if failed else f"✓ {command} succeeded"
    print(f"{status} in {elapsed:.0f}s")

    if len(report) > MAX_OUTPUT_LINES:
        hidden = len(report) - MAX_OUTPUT_LINES
        report = report[:MAX_OUTPUT_LINES] + [f"... {hidden} more lines hidden. See the full log."]
    for line in report:
        print(line)

    print(f"Full log: {LOG_PATH}")
    sys.exit(result.returncode)


if __name__ == "__main__":
    main()
