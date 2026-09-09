set dotenv-load

export UBSAN_OPTIONS := 'print_stacktrace=1'

configs := ''
targets := ''

# Fedora already has the required packages
[doc]
initialize-host:
    sudo apt -y update
    sudo apt -y install podman

# TODO: The only reason we have a .bazelversion is because fish completions can't be generated without it
# https://github.com/bazelbuild/bazelisk/issues/718#issuecomment-3435688542
[doc]
bazel-completions:
    bazelisk completion fish > ~/.config/fish/completions/bazelisk.fish

_cmd cmd *targets=targets:
    bazel \
        {{ cmd }} \
        {{ configs }} \
        {{ targets }}

build *targets=targets: (_cmd "build" targets)

test *targets=targets: (_cmd "test" targets)

run target=targets: (_cmd "run" target)

# HACK: hedron_compile_commands doesn't support C++20 modules, have to build first
[doc]
compile_commands: (build "//src/...") (run "@hedron_compile_commands//:refresh_all")

docs: (run "//docs:serve")

pre-commit:
    prek run --all-files

pre-commit-install:
    prek install

pre-commit-update:
    prek autoupdate

clean *args:
    bazel clean {{ args }}

update-submodules:
    git submodule update --init --recursive --remote
