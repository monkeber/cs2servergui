FROM ubuntu:24.04

ARG TARGETARCH
ARG QT_VERSION=6.10.2
ARG RUST_TOOLCHAIN=stable

ENV LANG=C.UTF-8
ENV PATH="/home/ubuntu/.cargo/bin:/opt/dev-tools/bin:${PATH}"
ENV Qt6_ROOT=/opt/qt/desktop

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential ca-certificates curl fonts-dejavu-core gdb git ninja-build \
        pkg-config python3-venv wayland-utils \
        libdbus-1-dev libegl1-mesa-dev libfontconfig1-dev libgl1-mesa-dev \
        libssl-dev libvulkan-dev libwayland-dev libxkbcommon-dev libxkbcommon-x11-dev \
        libxcb-cursor0 libxcb1-dev mesa-vulkan-drivers wayland-protocols \
    && rm -rf /var/lib/apt/lists/* \
    && install -d -m 0700 -o ubuntu -g ubuntu /home/ubuntu/.xdg-runtime

# Install CMake from Kitware's stable Noble repository.
RUN install -d -m 0755 /etc/apt/keyrings \
    && curl -fsSL https://apt.kitware.com/keys/kitware-archive-latest.asc \
        -o /etc/apt/keyrings/kitware-bootstrap.asc \
    && printf '%s\n' 'deb [signed-by=/etc/apt/keyrings/kitware-bootstrap.asc] https://apt.kitware.com/ubuntu/ noble main' \
        > /etc/apt/sources.list.d/kitware.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends cmake kitware-archive-keyring \
    && sed -i 's|/etc/apt/keyrings/kitware-bootstrap.asc|/usr/share/keyrings/kitware-archive-keyring.gpg|' \
        /etc/apt/sources.list.d/kitware.list \
    && rm -f /etc/apt/keyrings/kitware-bootstrap.asc \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m venv /opt/dev-tools \
    && pip install --no-cache-dir aqtinstall==3.3.0 conan==2.34.0

# Install the project's Qt version from the Qt distribution archives.
RUN case "${TARGETARCH}" in \
        amd64) qt_host=linux; qt_arch=linux_gcc_64; qt_dir=gcc_64 ;; \
        arm64) qt_host=linux_arm64; qt_arch=linux_gcc_arm64; qt_dir=gcc_arm64 ;; \
        *) echo "Unsupported Qt SDK architecture: ${TARGETARCH}" >&2; exit 1 ;; \
    esac \
    && aqt install-qt "${qt_host}" desktop "${QT_VERSION}" "${qt_arch}" \
        --outputdir /opt/qt --archives qtbase qtdeclarative qtsvg qtwayland icu \
    && ln -s "/opt/qt/${QT_VERSION}/${qt_dir}" "${Qt6_ROOT}" \
    && test -f "${Qt6_ROOT}/lib/cmake/Qt6/Qt6Config.cmake" \
    && test -f "${Qt6_ROOT}/lib/cmake/Qt6WaylandClient/Qt6WaylandClientConfig.cmake"

RUN install -d -m 0700 -o ubuntu -g ubuntu /home/ubuntu/.codex

USER ubuntu

RUN curl --fail --silent --show-error --location https://sh.rustup.rs -o /tmp/rustup-init.sh \
    && sh /tmp/rustup-init.sh -y --no-modify-path --profile minimal \
        --default-toolchain "${RUST_TOOLCHAIN}" --component rustfmt --component clippy \
    && rm /tmp/rustup-init.sh
