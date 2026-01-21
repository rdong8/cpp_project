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

In the [devcontainer configuration](.devcontainer/devcontainer.json), a volume has been configured for Conan. This helps persist Conan's cache and build profile even when the container is destroyed.

#### Profile

Conan profiles specify toolchain details for building packages. There are 2 kinds:

- Build profile: describes the system where the build is happening
  - Used to build tools that will run on the build machine during the build process like CMake, Ninja, etc.
- Host profile: describes the system where the built binaries will run
  - Used to build your project and its dependencies

In other words, the build profile is used to build *tools*, whereas the host profile is used to build *your project*. Conan can automatically detect an appropriate build profile for you:

```bash
just create-conan-profile build
```

You can run this once and will basically never have to touch it again unless the toolchain provided by the container image changes (ie. you switch to a newer Fedora image).

On the other hand, the host profile specifies the things you care about like the C++ version, compiler, standard library, etc. you plan on using. An example is given in [conan/profiles/host](conan/profiles/host). The project is configured to use this profile by default. If you want to copy it into the standard Conan config location you can run:

```bash
just copy-conan-profile host
```

And then set the `conan_host_profile` variable in the justfile to its name (`host`) instead of a relative path.

You can edit a profile in the config location with `just edit-conan-profile host`.

Note that if your `compiler.version` in your host profile is too new, you may get an error from Conan. Just [edit `~/.conan2/settings.yml`](http://docs.conan.io/2/knowledge/faq.html#error-invalid-setting) and add it there.

#### Build Dependencies

This step needs to be run each time anything in [`conan/`](./conan) is modified. Build the project's C++ dependencies with Conan:

```bash
just conan-install
```

## Configure

Run the CMake configure. Generates the underlying build system files.

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

After cleaning, you have to re-run everything from `just conan-install` and after.

Clean Python files as well:

```bash
just clean-all
```

After this you also have to run `just venv py-deps`.

You can also delete all installed Conan packages matching a pattern:

```bash
just clean-conan 'boost/*'
just clean-conan # Removes everything
```
