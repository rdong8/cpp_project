# Monorepo

Monorepo for my personal projects. Also serves as a demo for doing things the "right way". This always means using the most correct, most modern, most powerful tool for the job. There is no such thing as overkill.

## Initialize

*On the host*:

```fish
git clone https://github.com/rdong8/cpp_project.git
cd cpp_project/
```

Install the following dependencies on the host:

```fish
sudo dnf -y install cpp podman
```

Then run `id` on the host to determine your user's UID and GID. Use that to fill in the `build.dockerfile.args.HOST_UID` and `build.dockerfile.args.HOST_GID` values in the [devcontainer.json](.devcontainer/devcontainer.json) file.

Then set the `dotfiles.repository` setting in VS Code to your dotfiles repository. Note that your install script MUST add Linuxbrew to the fish `PATH`.

Then build the devcontainer. All commands after this point are to be run *in the devcontainer*, not on the host.

## Docs

Build and serve the doxygen documentation:

```fish
just docs
```

## Pre-Commit

Registers pre-commit hooks to run automatically.

```bash
just pre-commit-install
```

## Clean

Clean the build directory and Conan generated files:

```bash
just clean
```

After cleaning, you have to re-run everything from `just conan-install` and after.

You can also delete all installed Conan packages matching a pattern:

```bash
just clean-conan 'boost/*'
just clean-conan # Removes everything
```
