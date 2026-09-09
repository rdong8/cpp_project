set dotenv-load

export UBSAN_OPTIONS := 'print_stacktrace=1'

configs := ''
targets := ''

# Command that will be invoked to open the `index.html` from the documentation.
# If running locally, you can use `xdg-open` to automatically select your system's default browser
browser := env("EDITOR", "code")

# Fedora already has the required packages
initialize-host:
    sudo apt -y update
    sudo apt -y install podman

_cmd cmd:
    bazel \
        {{ cmd }} \
        {{ configs }} \
        {{ targets }}

build: (_cmd "build")

test: (_cmd "test")

run: (_cmd "run")

pre-commit:
    prek run --all-files

pre-commit-install:
    prek install

pre-commit-update:
    prek autoupdate

clean:
    rm -rf \
        bazel-*/

update-submodules:
    git submodule update --init --recursive --remote
