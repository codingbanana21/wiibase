# ============================================================
#  Wii Homebrew Development Environment
#  Base:  devkitpro/devkitppc (official devkitPro image)
#  Adds:  GRRLIB + all its dependencies
# ============================================================
#
# Using devkitPro's own published image as the base avoids all
# the fragile install-script/wget issues. devkitPPC, libogc,
# wiiuse, libfat, and the full Wii toolchain are already present.
#
FROM devkitpro/devkitppc:latest

LABEL maintainer=""
LABEL description="Wii homebrew dev environment (devkitPPC + GRRLIB)"

# ── Extra system packages ────────────────────────────────────
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    make \
    cmake \
    wget \
    curl \
    zip \
    unzip \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# ── GRRLIB dependencies (via dkp-pacman) ────────────────────
RUN dkp-pacman --noconfirm -Syu \
    && dkp-pacman --noconfirm -S \
        ppc-libpng \
        ppc-libjpeg-turbo \
        ppc-freetype \
        ppc-zlib \
        ppc-bzip2 \
        ppc-brotli \
    && dkp-pacman --noconfirm -Sc

# ── Environment variables ────────────────────────────────────
ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITPPC=/opt/devkitpro/devkitPPC
ENV PATH="${DEVKITPPC}/bin:${DEVKITPRO}/tools/bin:${PATH}"

# ── GRRLIB (build from source) ───────────────────────────────
# GRRLIB is a 2D/3D rendering library for Wii homebrew built on
# top of GX. It is not in the devkitPro pacman repo, so we clone
# the official repo and build + install it manually.
RUN git clone --depth 1 https://github.com/GRRLIB/GRRLIB.git /tmp/GRRLIB \
    && cd /tmp/GRRLIB/GRRLIB \
    && make clean \
    && make \
    && make install \
    && rm -rf /tmp/GRRLIB

# ── Working directory for your project ──────────────────────
WORKDIR /project

# ── Default command ──────────────────────────────────────────
CMD ["/bin/bash"]
