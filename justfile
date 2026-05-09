export BUILD_DIR := 'build'

# Set to any non-empty string for extra output
verbose := ''

cmake_configure_preset := 'default'
cmake_build_preset := 'default'

config:
    cmake \
        -S . \
        -B {{ BUILD_DIR }} \
        --preset {{ cmake_configure_preset }}

build target='all':
    cmake \
        --build {{ BUILD_DIR }} \
        --preset {{ cmake_build_preset }} \
        {{ if target != '' { '-t ' + target } else { '' } }} \
        {{ if verbose != '' { '-v' } else { '' } }}

clean:
    rm -rf {{ BUILD_DIR }}
