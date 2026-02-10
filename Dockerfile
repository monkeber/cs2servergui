FROM ubuntu:24.04

# Avoid interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    ninja-build \
    pipx \
    libgl1-mesa-dev \
    libxcb* \
    libwayland-dev \
    wayland-protocols \
    libxkbcommon-dev \
    locales \
    locales-all

ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

RUN apt-get remove cmake

RUN apt-get update && apt-get install wget
RUN wget https://apt.kitware.com/kitware-archive.sh -O kitware-archive.sh
RUN chmod +x ./kitware-archive.sh && ./kitware-archive.sh
RUN apt-get update && apt-get install -y cmake

# RUN pipx ensurepath
RUN pipx install conan
ENV PATH="/root/.local/bin:${PATH}"

RUN mkdir /qtroot && \
    cd /qtroot && \
    git clone --branch v6.10.2 https://code.qt.io/qt/qt5.git qt && \
    cd qt && \
    ./init-repository -submodules qtwayland,qtdeclarative,qtbase,qtsvg

RUN mkdir -p /qtroot/install && \
    mkdir -p /qtroot/build && \
    cd /qtroot/build && \
    ../qt/configure -submodules qtwayland,qtdeclarative,qtbase,qtsvg -skip qtimageformats -static -prefix /qtroot/install

RUN cd /qtroot/build && \
    cmake --build . --parallel && \
    cmake --install .

RUN rm -rf /qtroot/build
RUN rm -rf /qtroot/qt

# Set working directory
WORKDIR /build

CMD ["sh", "-c", "\
    cmake --version && \
    conan --version && \
    cmake --preset release -DQt6_ROOT=/qtroot/install && \
    cmake --build --preset release && \
    cd build/release && \
    cmake --install . \
"]
