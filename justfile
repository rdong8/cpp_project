set dotenv-load

# hx, vim, cursor, etc.
export EDITOR := "code"

project := "cpp_project"

# Set to any non-empty string for extra output
verbose := ""

build_dir := "build"

build_type := "Release"
preset := "conan-" + shell('echo ' + build_type + ' | tr "[:upper:]" "[:lower:]"')

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
browser := "code"

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

conan-install requires="":
    BUILD_DIR={{ build_dir }} \
        {{ conan }} \
            install \
            -b missing \
            -pr:b {{ conan_build_profile }} \
            -pr:h {{ conan_host_profile }} \
            {{ if requires != "" { "--requires=\"" + requires + "\"" } else { "." } }}

# https://github.com/conan-io/conan/issues/17333#issuecomment-3084941843
conan-install-mold:
    just conan-install "mold/[*]"

conan-install-all:
    just conan-install-mold conan-install

config config_preset="conan-default":
    cmake \
        -S . \
        -B {{ build_dir }} \
        --preset {{ config_preset }}

build target=default_build_target:
    cmake \
        --build {{ build_dir }} \
        --preset {{ preset }} \
        {{ if target != "" { "-t " + target } else { "" } }} \
        {{ if verbose != "" { "-v" } else { "" } }} \
        | tee {{ log }}

run target=default_run_target *args=default_args:
    ./{{ build_dir }}/src/{{ build_type }}/{{ if target == "all" { project } else { target } }} \
        {{ args }} \
        | tee {{ log }}

docs:
    {{ browser }} $(realpath {{ build_dir }})/docs/html/index.html

test:
    ctest \
        --preset {{ preset }} \
        {{ if verbose != "" { "--extra-verbose" } else { "" } }}

pre-commit:
    uv run pre-commit install

clean:
    rm -rf \
        {{ build_dir }} \
        *conan*.sh

clean-python:
    rm -rf \
        uv.lock \
        .venv

clean-all:
    just clean clean-python

clean-conan:
    {{ conan }} remove "*"

update-submodules:
    git submodule update --init --recursive
