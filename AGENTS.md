# AGENTS.md

Always means using the most correct, most modern, most powerful tool for the job. There is no such thing as overkill.

## Performance

## Concurrency

## Style

## Testing

- Use `-fno-access-control` for unit testing instead of adding member functions that shouldn't be public

## Bazel

- Use `implementation_deps` for C++ targets
- Use the fish shell for all shell targets. Use `argparse` and named arguments only.
- Never touch the system compiler for experimentation/testing. Make a temporary bazel target to play around with.

## Meta

- Avoid making compromises without consulting with me.
- Avoid workarounds/hacks, again consult with me first. For example, for third party dependencies, I often prefer to patch them instead of hacking my own code.
