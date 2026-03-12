# AGENTS.md - Agent Coding Guidelines for writevt

## Project Overview

writevt is a small C utility that sends text to a terminal device using the TIOCSTI ioctl. It allows injecting text into a TTY as if typed by the user.

- **Language**: C
- **Files**: Single file project (writevt.c)
- **Platform**: Linux (requires TIOCSTI support)

## Build Commands

### Compilation

```bash
# Basic compilation
gcc -Wall -Wextra -pedantic -o writevt writevt.c

# With debug symbols
gcc -g -Wall -Wextra -pedantic -o writevt writevt.c

# Release build with optimizations
gcc -O2 -Wall -Wextra -pedantic -o writevt writevt.c
```

### Cleaning

```bash
rm -f writevt
```

### Testing

**No test suite exists for this project.** There are no automated tests.

To manually test:
```bash
# Compile first
gcc -Wall -Wextra -pedantic -o writevt writevt.c

# Run with a valid TTY (requires appropriate permissions)
sudo ./writevt /dev/tty1 "echo hello"
```

### Running a Single Test

Not applicable - no tests exist.

---

## Code Style Guidelines

### General Principles

- Write clean, portable C code compatible with POSIX systems
- Prioritize readability and maintainability
- Use defensive programming practices
- Keep functions small and focused

### Formatting

- **Indentation**: 4 spaces (no tabs)
- **Line length**: Keep lines under 100 characters when reasonable
- **Braces**: K&R style (opening brace on same line)
  ```c
  if (condition) {
      do_something();
  }
  ```
- **Spacing**: Space after keywords (if, while, for), no space after function names

### Includes

Order includes from most specific to least specific:
1. Project headers (none for this project)
2. Standard C library headers
3. System headers

```c
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
```

### Naming Conventions

- **Functions**: lowercase_with_underscores (e.g., `process_input`)
- **Variables**: lowercase_with_underscores (e.g., `term`, `text`)
- **Constants**: UPPER_CASE_WITH_UNDERSCORES (e.g., `BUFFER_SIZE`)
- **Global variables**: Avoid; use static when necessary

### Types

- Use standard C types: `int`, `char`, `size_t`, `ssize_t`
- Prefer `int` for return codes (0 = success, non-zero = error)
- Use `char*` for strings
- Avoid unsigned types unless bit operations are needed

### Functions

- Use `static` for internal (file-local) functions
- Use `int` return type for functions that can fail
- Return 0 on success, positive error code on failure
- Use `void` for functions that always succeed

Example:
```c
static int validate_args(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: insufficient arguments\n");
        return 1;
    }
    return 0;
}
```

### Error Handling

- Use `fprintf(stderr, ...)` for error messages
- Use `perror()` for system call failures (includes errno context)
- Return appropriate exit codes:
  - `0` = success
  - `1` = general error
  - `2` = usage error (invalid arguments)

```c
fd = open(term, O_RDONLY);
if (fd < 0) {
    perror(term);
    fprintf(stderr, "%s: could not open tty\n", progname);
    return 1;
}
```

### Global Variables

Minimize global variables. When required, declare as `static`:

```c
static char *progname;
```

### Comments

- Use comments to explain **why**, not **what**
- Keep comments up-to-date with code changes
- Avoid obvious comments

### Portability

- Use POSIX/standard C functions only
- Avoid Linux-specific extensions unless necessary
- Test on multiple systems if portability matters

---

## Development Notes

### Permissions

- Writing to TTY devices typically requires root or ownership of the device
- Use `sudo` when needed: `sudo ./writevt /dev/tty1 "text"`

### Dependencies

- No external dependencies - uses only standard C library and Linux syscalls

### Future Improvements (for reference)

If expanding the project, consider:
1. Adding a Makefile for standardized builds
2. Adding unit tests with a simple test framework
3. Adding CI/CD (GitHub Actions)
4. Adding more error checking and validation
5. Supporting command-line flags (-h, -v, etc.)

---

## Quick Reference

| Task | Command |
|------|---------|
| Build | `gcc -Wall -Wextra -pedantic -o writevt writevt.c` |
| Clean | `rm -f writevt` |
| Run | `sudo ./writevt /dev/tty "text"` |
| Check for mem issues | `valgrind ./writevt /dev/tty "text"` |
