set dotenv-load

# hx, vim, cursor, etc.
export EDITOR := "code"
export BUILD_DIR := "build"

project := "cpp_project"

# Set to any non-empty string for extra output
verbose := ""

# The build type for your own code
build_type := "Debug"

# The build type for your dependencies, as specified in the conan profiles
conan_build_type := "Release"

# We're using a multi-configuration generator so there's only one configure preset
cmake_configure_preset := "conan-default"
cmake_build_preset := "conan-" + shell('echo ' + build_type + ' | tr "[:upper:]" "[:lower:]"')
cmake_test_preset := cmake_build_preset

log := "/tmp/out.log"

conan := "uv run conan"

conan_build_profile := "build"
conan_host_profile := "host"

# The default target to build if none is provided to the `build` recipe
default_build_target := "all"

# Default target to run if none is provided to the `run` recipe
default_run_target := project

# Default arguments to provide when running the executable if none are provided to the `run` recipe
default_args := ""

# Command that will be invoked to open the `index.html` from the documentation.
# If running locally, you can use `xdg-open` to automatically select your system's default browser
browser := EDITOR

# Fedora already has the required packages
initialize-host:
    sudo apt -y update
    sudo apt -y install podman

venv:
    uv venv

py-deps reinstall="0":
    uv pip install --upgrade -e . \
        {{ if reinstall == "1" { "--reinstall" } else { "" } }}

# This only creates the profile, you still need to edit it to contain the details for your compiler and language
conan-profiles force="":
    {{ conan }} profile detect --name {{ conan_host_profile }} \
        {{ if force != "" { "--force" } else { "" } }}
    {{ conan }} profile detect --name {{ conan_build_profile }} \
        {{ if force != "" { "--force" } else { "" } }}

edit-conan-profile profile:
    {{ EDITOR }} $({{ conan }} config home)/profiles/{{ profile }}

# https://github.com/conan-io/conan/issues/17333#issuecomment-3084941843
conan-install-mold:
    {{ conan }} \
        install \
        -b missing \
        -pr:b {{ conan_build_profile }} \
        -pr:h {{ conan_host_profile }} \
        -s build_type={{ conan_build_type }} \
        --requires='mold/[*]'

# &:build_type=XXX means "use the build type in the profile for my dependencies but build my code with XXX"
conan-install:
    {{ conan }} \
        install \
        -b missing \
        -pr:b {{ conan_build_profile }} \
        -pr:h {{ conan_host_profile }} \
        -s '&:build_type={{ build_type }}' \
        -s build_type={{ conan_build_type }} \
        .

conan-install-all:
    just conan-install-mold conan-install

config:
    cmake \
        -S . \
        -B {{ BUILD_DIR }} \
        --preset {{ cmake_configure_preset }} \
        | tee {{ log }}

build target=default_build_target:
    cmake \
        --build {{ BUILD_DIR }} \
        --preset {{ cmake_build_preset }} \
        {{ if target != "" { "-t " + target } else { "" } }} \
        {{ if verbose != "" { "-v" } else { "" } }} \
        | tee {{ log }}

run target=default_run_target *args=default_args:
    ./{{ BUILD_DIR }}/src/{{ build_type }}/{{ if target == "all" { project } else { target } }} \
        {{ args }} \
        | tee {{ log }}

docs:
    {{ browser }} $(realpath {{ BUILD_DIR }})/docs/html/index.html

test:
    ctest \
        --preset {{ cmake_test_preset }} \
        {{ if verbose != "" { "--extra-verbose" } else { "" } }} \
        | tee {{ log }}

pre-commit:
    uv run pre-commit install

clean:
    rm -rf \
        {{ BUILD_DIR }} \
        *conan*.sh \
        CMakeUserPresets.json

clean-python:
    rm -rf \
        uv.lock \
        .venv

clean-all:
    just clean clean-python

# Cleans cached conan packages
clean-conan:
    {{ conan }} remove '*'

update-submodules:
    git submodule update --init --recursive
