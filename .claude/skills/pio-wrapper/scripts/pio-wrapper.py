#!/usr/bin/env python3
"""
PlatformIO wrapper script that filters output to reduce token usage.
- On success: returns minimal output
- On error: returns only error lines (not the entire build log)
"""

import subprocess
import sys
import re

def is_error_line(line):
    """Determine if a line is an error line."""
    # Check for error keywords (case insensitive)
    if re.search(r'\berror\b', line, re.IGNORECASE):
        return True
    # Check for ANSI color codes that indicate red text (error)
    # Red text in terminal is typically: \033[91m or \033[31m
    if '\033[91m' in line or '\033[31m' in line or '\033[1;31m' in line:
        return True
    # Check for "FAILED" keyword
    if re.search(r'\bfailed\b', line, re.IGNORECASE):
        return True
    return False

def extract_errors(output):
    """Extract error lines from pio output."""
    lines = output.split('\n')
    error_lines = []

    for line in lines:
        if is_error_line(line):
            # Clean ANSI color codes for cleaner output
            clean_line = re.sub(r'\033\[[0-9;]*m', '', line)
            if clean_line.strip():
                error_lines.append(clean_line)

    return error_lines

def main():
    """Run PlatformIO command and filter output to reduce token usage."""
    # Get all arguments passed to this script (excluding script name)
    pio_args = sys.argv[1:]

    try:
        # Run pio with all arguments
        result = subprocess.run(
            ['pio'] + pio_args,
            capture_output=True,
            text=True,
            check=False
        )

        # On success, return minimal output
        if result.returncode == 0:
            print("✓ PlatformIO command succeeded")
            sys.exit(0)

        # On error, extract and return only error lines
        error_output = result.stdout + result.stderr
        error_lines = extract_errors(error_output)

        if error_lines:
            print("✗ PlatformIO command failed. Errors:")
            for line in error_lines:
                print(line)
        else:
            # If no specific error lines found, print last few lines of output
            lines = error_output.split('\n')
            relevant_lines = [l for l in lines if l.strip()][-10:]
            print("✗ PlatformIO command failed:")
            for line in relevant_lines:
                print(line)

        sys.exit(result.returncode)

    except FileNotFoundError:
        msg = (
            "✗ Error: 'pio' command not found. "
            "Ensure PlatformIO is installed and in PATH."
        )
        print(msg, file=sys.stderr)
        sys.exit(1)
    except OSError as e:
        print(f"✗ Error running pio wrapper: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
