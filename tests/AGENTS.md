# AGENTS.md

- Use module implementation partition units to write unit tests. This allows testing of internal APIs of a project that may not be `export`'ed.
- Use `-fno-access-control` for unit testing hidden members instead of incorrectly making them visible.

## Test Selection

Do not write stupid tests. Examples:

- Don't write an assertion with constants for both operands, ie. `static_assert(sizeof(MessageFrame) == 8UZ)`. If this is required by some protocol, then it belongs next to the struct declaration, not in the unit tests. Else you're just comparing a compile time constant against a magic number.
