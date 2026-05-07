FROM docker.io/fedora:latest

# For now, only install the following with the system package manager:
# - Dependencies of linuxbrew itself (curl, file, procps-ng)
# - gcc-c++: system toolchain needed for linuxbrew's LLVM
# - fish: need to set user's shell when we create it
# - which: to find fish for the useradd
RUN <<EOF
  dnf update -y
  dnf install -y \
    @development-tools \
    curl \
    file \
    fish \
    gcc-c++ \
    procps-ng \
    which
  dnf clean all
  rm -rf /var/cache/dnf
EOF

# Add user
ARG REMOTE_USER
ARG HOST_UID
ARG HOST_GID
RUN <<EOF
  groupadd --gid ${HOST_GID} ${REMOTE_USER}
  useradd -ms $(which fish) --uid ${HOST_UID} --gid ${HOST_GID} ${REMOTE_USER}
EOF
USER ${REMOTE_USER}

# Get homebrew binary
ARG HOMEBREW_PREFIX=/home/linuxbrew/.linuxbrew
COPY \
  --from=docker.io/homebrew/brew:latest \
  --chown=${REMOTE_USER} \
  ${HOMEBREW_PREFIX} \
  ${HOMEBREW_PREFIX}

ARG FISH_CONFIG=/home/${REMOTE_USER}/.config/fish

# Homebrew-related things
RUN <<EOF
  # Setup fish config
  mkdir -p ${FISH_CONFIG}

cat <<EOF2 >> ${FISH_CONFIG}/config.fish
eval (${HOMEBREW_PREFIX}/bin/brew shellenv)
fish_add_path (brew --prefix rustup)/bin
EOF2

  # Install brew packages
  eval "$(${HOMEBREW_PREFIX}/bin/brew shellenv)"
  # Note that LLVM is needed for wild
  brew install \
    bat \
    btop \
    cargo-binstall \
    fastfetch \
    fd \
    fzf \
    gawk \
    git-delta \
    helix \
    jq \
    just \
    llvm \
    prek \
    ripgrep \
    rustup \
    terror/tap/just-lsp \
    tree \
    wget \
    zellij

  # Fish completions
  # Only necessary if not builtin: https://github.com/fish-shell/fish-shell/tree/master/share/completions
  # prek: https://prek.j178.dev/installation/#shell-completion
  #   TODO: https://github.com/j178/prek/issues/1992
  mkdir -p ${FISH_CONFIG}/completions \
  COMPLETE=fish prek > ${FISH_CONFIG}/completions/prek.fish
EOF

# Wild linker
RUN <<EOF
  cargo binstall wild-linker
  mkdir -p ~/.cargo

cat <<EOF2 >> ~/.cargo/config.toml
[target.x86_64-unknown-linux-gnu]
linker = "clang"
rustflags = ["-Clink-arg=--ld-path=wild"]
EOF2

EOF
