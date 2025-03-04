set dotenv-load

project := "cpp_project"

# Set to any non-empty string for extra output
verbose := "True"

build_dir := "build"

# How to build your code
# 	Source: https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html
build_type := "Debug"

conan := "uv run conan"

conan_build_profile := "default"
conan_host_profile := "default"

# The default target to build if none is provided to the `build` recipe
default_build_target := "all"

# Default target to run if none is provided to the `run` recipe
default_run_target := project

# Default arguments to provide when running the executable if none are provided to the `run` recipe
default_args := ""

# Command that will be invoked to open the `index.html` from the documentation.
# ie. Set to `firefox` so that docs are opened with `firefox index.html`
default_browser := "xdg-open"

venv:
	uv venv --python 3.13

py-deps reinstall="0":
	uv pip install --upgrade -e . \
	    {{ if reinstall == "1" { "--reinstall" } else { "" } }}

# This only creates the profile, you still need to edit it to contain the details for your compiler and language
conan-profile:
	{{ conan }} profile detect --force

conan-deps:
	BUILD_DIR={{ build_dir }} \
		{{ conan }} \
			install . \
			-b missing \
			-pr:b {{ conan_build_profile }} \
			-pr:h {{ conan_host_profile }} \
			-s build_type={{ build_type }}

config:
	cmake \
		-S . \
		-B {{ build_dir }} \
		--preset Default

build target=default_build_target:
	cmake \
		--build {{ build_dir }} \
		--preset {{ build_type }} \
		{{ if target != "" { "-t " + target } else { "" } }} \
		{{ if verbose != "" { "-v" } else { "" } }}

run target=default_run_target *args=default_args:
    ./{{ build_dir }}/src/{{ build_type }}/{{ if target == "all" { project } else { target } }} {{ args }}

docs browser=default_browser:
    {{ browser }} $(realpath {{ build_dir }})/docs/html/index.html

test:
	ctest \
		--preset {{ build_type }} \
		{{ if verbose != "" { "--extra-verbose" } else { "" } }}

pre-commit:
	uv run pre-commit install

clean:
	rm -rf {{ build_dir }}

clean-conan:
	{{ conan }} remove "*"
