#!/usr/bin/env fish

# This script installs the bundle given by the Brewfile that the image stages append packages to.
# Installing brew packages here ensures they end up in the brew cache volume, since it cannot be mounted during the image build.
# Not hermetic but that's fine since we use Bazel for the important stuff, these are mostly just CLI tools
# It may seem that `postCreateCommand` would be more appropriate, but that is executed *before* VS Code's `dotfiles.installCommand`, which is expected to add brew to PATH.
# Instead, this runs on every container start, but early-exits if it has already been run before

argparse '/Brewfile=' -- $argv
or return

if not set -ql _flag_Brewfile
    echo "Required argument missing: --Brewfile" >&2
    return 1
end

# IMPORTANT ASSUMPTION: your `dotfiles.installCommand` must add brew to path, ie. by way of creating a fish.config that evals `brew shellenv`
if not type -q brew
    echo 'brew not in path, did your `dotfiles.installCommand` complete successfully?' >&2
    return 1
end

set -l Brewfile (mktemp)

# This fails if the given Brewfile either doesn't exist or has already been moved by a concurrently running start script
if not mv $_flag_Brewfile $Brewfile 2>/dev/null
    rm -f $Brewfile
    return 0
end

if brew bundle --file=$Brewfile
    rm -f $Brewfile
else
    set -l exit_code $status
    echo "brew bundle failed with exit code $exit_code; restoring $_flag_Brewfile"
    mv $Brewfile $_flag_Brewfile
    return $exit_code
end
