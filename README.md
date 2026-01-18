# cpp_project

Template for a modern C++ project using CMake.

Also read the [justfile](./justfile) to see available commands.

## Notes

- Only Linux is supported
- This project uses [devcontainers](https://containers.dev/) to avoid dependencies polluting the host. Specifically, we use [podman](https://podman.io/) instead of Docker, which enables the devcontainer to run on a host that is itself containerized (ie. a [GCP Cloud Workstation](https://cloud.google.com/workstations?hl=en) instance)
- The devcontainer uses [`brew`](https://brew.sh/) for a few reasons:
  - We use a non-root user in the container so you can't use the system package manager
  - The LLVM build in Fedora's repos lacks debug symbols
- The devcontainer is setup to use [fish](https://fishshell.com/) as the default shell

## Initialize

*On the host*:

```bash
git clone --recurse-submodules https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

Also make sure you have [`podman`](https://podman.io/docs/installation#installing-on-linux) installed on the host. For example:

```bash
# Fedora
sudo dnf -y install podman

# Ubuntu
sudo apt update
sudo apt -y install podman
```

Then run `id` on the host to determine your user's UID and GID. Use that to fill in the `build.dockerfile.args.HOST_UID` and `build.dockerfile.args.HOST_GID` values in the [devcontainer.json](.devcontainer/devcontainer.json) file.

Then build the devcontainer. All commands after this point are to be run *in the devcontainer*, not on the host.

## Dependencies

### Python

```bash
just venv # Creates a virtual environment
just py-deps # Installs the Python dependencies. Use `just py-deps 1` to force a reinstall.
```

### Conan

#### Profile

In the [devcontainer configuration](.devcontainer/devcontainer.json), a volume has been configured for Conan's cache and configurations so this step only needs to be run once per devcontainer host.

Check if you already have a Conan profile with `just list-conan-profiles`.

If you haven't already created one, this command will create a default for you:

```bash
just create-conan-profile default
```

Then, edit the profile with `just edit-conan-profile default`. An example is given in [conan/profiles/default](conan/profiles/default).

Note that if your `compiler.version` is too new, you may get an error from Conan. Just [edit `~/.conan2/settings.yml`](http://docs.conan.io/2/knowledge/faq.html#error-invalid-setting) and add it there.

#### Build Dependencies

Now build the project's C++ dependencies with Conan:

```bash
just conan-install
```

## Configure

This command runs the CMake configure. Theoretically it only needs to be run once, since the `just build` command invokes `cmake --build` which should automatically reconfigure if necessary. In practice if you're having CMake issues, just try cleaning and rerunning this.

```bash
just config
```

At this point, you may want to restart clangd (`clangd: Restart language server` in VSCode command palette) so it picks up the new compile commands.

## Build

Build a target:

```bash
just build target-name
```

You can omit the target name to build everything.

The target will end up in `./build/src/path/to/target/build_type/target`.

## Docs

To open the documentation in the default browser (must be built first via `just build docs`):

```bash
just docs
```

You may also pass a command that will be used to open the `index.html` file:

```bash
just docs firefox
just docs 'flatpak run com.brave.Browser' # You need to use Flatseal to give the flatpak permission in this case
```

## Test

Run all tests:

```bash
just test
```

Any [flags](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html#testing-using-ctest) will be forwarded to `ctest`, for example:

```bash
just test -R math # Run tests matching regular expression "math"
```

## Pre-Commit

Registers pre-commit hooks to run automatically.

```bash
just pre-commit
```

## Clean

Clean the build directory and Conan generated files:

```bash
just clean
```

Clean Python files as well:

```bash
just clean-all
```

You'll need to make the project's Conan dependencies and run the CMake config again with `just conan-deps config`.

You can also delete all installed Conan packages matching a pattern:

```bash
just clean-conan 'boost/*'
just clean-conan # Removes everything
```
