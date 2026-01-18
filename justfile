set dotenv-load

# hx, vim, cursor, etc.
export EDITOR := 'code'
export BUILD_DIR := 'build'

project := 'cpp_project'

# Set to any non-empty string for extra output
verbose := ''

# The build type for your own code
build_type := 'Debug'

# The build type for your dependencies, as specified in the conan profiles
conan_build_type := 'Release'

# We're using a multi-configuration generator so there's only one configure preset
cmake_configure_preset := 'conan-default'
cmake_build_preset := 'conan-' + shell('echo ' + build_type + ' | tr "[:upper:]" "[:lower:]"')
cmake_test_preset := cmake_build_preset

log := '/tmp/out.log'

# For cross-compilation you want separate host and build profiles
conan_build_profile := 'default'
conan_host_profile := conan_build_profile

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
        {{ if reinstall == '1' { '--reinstall' } else { '' } }}

list-conan-profiles:
    conan profile list

create-conan-profile name force='':
    conan profile detect --name {{ name }} \
        {{ if force != '' { '--force' } else { '' } }}

edit-conan-profile profile:
    {{ EDITOR }} $(conan config home)/profiles/{{ profile }}

# build_type=XXX means "use this build type for my dependencies"
# &:build_type=XXX means "use this build type for my code"
conan-install:
    conan \
        install \
        -b missing \
        -pr:b {{ conan_build_profile }} \
        -pr:h {{ conan_host_profile }} \
        -s build_type={{ conan_build_type }} \
        -s '&':build_type={{ build_type }} \
        conan

config:
    cmake \
        -S . \
        -B {{ BUILD_DIR }} \
        --preset {{ cmake_configure_preset }} \
        | tee {{ log }}

build target='all':
    cmake \
        --build {{ BUILD_DIR }} \
        --preset {{ cmake_build_preset }} \
        {{ if target != '' { '-t ' + target } else { '' } }} \
        {{ if verbose != '' { '-v' } else { '' } }} \
        | tee {{ log }}

docs:
    {{ browser }} $(realpath {{ BUILD_DIR }})/docs/html/index.html

test *args:
    ctest \
        --preset {{ cmake_test_preset }} \
        {{ if verbose != '' { '--extra-verbose' } else { '' } }} \
        {{ args }} \
        | tee {{ log }}

log:
    {{ EDITOR }} {{ log }}

pre-commit:
    prek install

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
clean-conan pattern='*' force='':
    conan remove '{{ pattern }}' \
        {{ if force != '' { '--confirm' } else { '' } }}

update-submodules:
    git submodule update --init --recursive
