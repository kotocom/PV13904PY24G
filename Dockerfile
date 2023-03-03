FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

ARG CMAKE_VERSION=3.15.2-0kitware1
ARG OPENOCD_VERSION=v0.11.0-rc2

# The values usually similar for all the default env. Othervise use
# docker image build --build-arg USER_ID=$(id -u ${USER}) --build-arg GROUP_ID=$(id -g ${USER})
ARG USER_ID=1000
ARG GROUP_ID=1000

RUN if [ ${USER_ID:-0} -ne 0 ] && [ ${GROUP_ID:-0} -ne 0 ]; then \
    groupadd -g ${GROUP_ID} koto &&\
    useradd -l -u ${USER_ID} -g koto koto &&\
    install -d -m 0755 -o koto -g koto /home/koto &&\
    chown --changes --silent --no-dereference --recursive \
          --from=33:33 ${USER_ID}:${GROUP_ID} \
        /home/koto \
;fi

RUN apt-get update && apt-get install -y apt-utils software-properties-common 2>&1

RUN add-apt-repository -y ppa:git-core/ppa && apt-get update && apt-get install -y \
    apt-utils \
    wget \
    git\
    vim \
    sudo \
    bc \
    gawk \
    curl \
    keychain \
    udev \
    pv \
    usbutils \
    doxygen \
    graphviz \
    gcc-arm-none-eabi \
    pkg-config \
    lsb-release \
    software-properties-common \
    # We need to install cmake version 3.13+ that is not in deb repo, thus we get it from others.
    && wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null \
    && add-apt-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" -y \
    && apt-get update \
    && apt-get install -y --no-install-recommends cmake-data=${CMAKE_VERSION} cmake=${CMAKE_VERSION} \
    2>&1

# Add no-paswd `sudo` to make things easier.
RUN adduser koto sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
RUN echo "StrictHostKeyChecking no" >> /etc/ssh/ssh_config

# Add ssh keychain login on container login. (id_rsa key name expected)
RUN echo "eval \`keychain --agents ssh --eval /home/koto/.ssh/id_rsa\`" > \
    /home/koto/.bashrc
# NOTE: Pass the rsa keys folder path to the docker: -v /home/${USER}/.ssh:/home/koto/.ssh
# skipped otherwise.

# Install openocd v11 from sources with stm-link.
RUN apt-get install -y \
    libtool \
    texinfo \
    libusb-dev \
    libusb-1.0.0-dev \
    libftdi-dev \
    autoconf \
    && \
    mkdir -p /usr/src; \
    cd /usr/src; \
    git clone https://github.com/ntfreak/openocd.git --branch ${OPENOCD_VERSION} 2>&1; \
    cd /usr/src/openocd; \
    git submodule init 2>&1; \
    git submodule update 2>&1; \
    ./bootstrap 2>&1; \
    ./configure --prefix=/opt/openocd 2>&1; \
    make -j 2>/dev/null; \
    make install; \
    ln -s /opt/openocd/bin/openocd /usr/local/bin/openocd;

# Export basic config to the .bashrc
RUN echo "#Config" >> /home/koto/.bashrc && \
    chown -R ${USER_ID}:${GROUP_ID} /home/koto/.bashrc && \
    echo "export PATH=/opt/openocd/bin:\$PATH" >> /home/koto/.bashrc && \
    echo "shopt -s histappend" >> /home/koto/.bashrc && \
    echo "HISTSIZE=10000" >> /home/koto/.bashrc && \
    echo "HISTFILESIZE=20000" >> /home/koto/.bashrc && \
    echo "HISTCONTROL=ignoreboth" >> /home/koto/.bashrc && \
    echo "cd ~/Oled/out" >> /home/koto/.bashrc && \
    git config --global --add safe.directory "*"

# Adding some commands to a history for the reverse search.
RUN echo "rm -rf * && cmake .. && make" > /home/koto/.bash_history && \
    echo "rm -rf * && cmake .. && make -j" >> /home/koto/.bash_history && \
    echo "rm -rf * && cmake .. && make -j run" >> /home/koto/.bash_history

## Propagate the rules for the STM NUCLEO-F756ZG devboard programmer.
RUN echo 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE:="0666", SYMLINK+="stlinkv2_%n", GROUP="plugdev"' \
    >> /etc/udev/rules.d/49-stm32l-discovery.rules

# Deliver locales for system and other deps.
RUN apt-get install -y locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    locale-gen
ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en

USER koto
