FROM ubuntu:24.04 AS base

# Avoid interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies. The list is might be excessive, a lot of dependencies were added
# when I tried to fix the issue of not detecting xcb integration plugin. A good place to improve
# in the future and reduce the dependency list.
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    ninja-build \
    pipx \
    libgl1-mesa-dev \
    libxcb-cursor-dev \
    libxcb-composite0-dev \
    libxcb-present-dev \
    libxcb-record0-dev \
    libxcb-res0-dev \
    libxcb-screensaver0-dev \
    libxcb-damage0-dev \
    libxcb-dpms0-dev \
    libxcb-dri2-0-dev \
    libxcb-dri3-dev \
    libxcb-xinput-dev \
    libxcb-glx0-dev \
    libgles2-mesa-dev \
    libegl1-mesa-dev \
    libegl-dev \
    libglx-dev \
    libgl-dev \
    libxcb-xinput-dev \
    libxcb1-dev \
    libxcb-glx0-dev \
    libxcb-keysyms1-dev \
    libxcb-image0-dev \
    libxcb-shm0-dev \
    libxcb-icccm4-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-shape0-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-util-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    libx11-xcb-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libwayland-dev \
    wayland-protocols \
    libxkbcommon-dev \
    locales \
    locales-all \
    fontconfig \
    libfontconfig-dev \
    libfreetype6 \
    libfreetype-dev

# Fix locale related warnings while building QT.
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Install latest CMake.
RUN apt-get remove cmake
RUN apt-get update && apt-get install wget
RUN wget https://apt.kitware.com/kitware-archive.sh -O kitware-archive.sh
RUN chmod +x ./kitware-archive.sh && ./kitware-archive.sh
RUN apt-get update && apt-get install -y cmake

# Install conan and make sure it's available.
RUN pipx install conan
ENV PATH="/root/.local/bin:${PATH}"

#
# Layer for only building qt.
#

FROM base AS qt_builder

RUN mkdir /qtroot && \
    cd /qtroot && \
    git clone --branch v6.10.2 https://code.qt.io/qt/qt5.git qt && \
    cd qt && \
    ./init-repository -submodules qtwayland,qtdeclarative,qtbase,qtsvg

RUN cd /qtroot/qt && git --no-pager log -1 > /qt_version.txt

RUN mkdir -p /qtroot/install && \
    mkdir -p /qtroot/build && \
    cd /qtroot/build && \
    ../qt/configure -submodules qtwayland,qtdeclarative,qtbase,qtsvg -skip qtimageformats -static -feature-freetype -fontconfig -prefix /qtroot/install

# This way we can check if xcb modules is properly detected.
# RUN cat /qtroot/build/config.summary | grep -A2 -i xcb

RUN cd /qtroot/build && \
    cmake --build . --parallel && \
    cmake --install .

#
# Copy built artifacts to another layer to save space in the final image.
#

FROM base AS builder

COPY --from=qt_builder /qtroot/install /qtroot/install
COPY --from=qt_builder /qt_version.txt /qt_version.txt

WORKDIR /build

#
# Build the application.
#

ARG USER_ID=1000
ARG GROUP_ID=1000
ENV USER_ID=${USER_ID}
ENV GROUP_ID=${GROUP_ID}

CMD ["sh", "-c", "\
    echo \"QT version:\" && \
    cat /qt_version.txt && \
    cmake --version && \
    conan --version && \
    cmake --preset release -DQt6_ROOT=/qtroot/install && \
    cmake --build --preset release && \
    cd build/release && \
    cmake --install . && \
    chown -R ${USER_ID}:${GROUP_ID} /build && \
    chown -R ${USER_ID}:${GROUP_ID} /install \
"]
