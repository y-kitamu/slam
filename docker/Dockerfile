FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

### setup user
ARG user
ARG uid
ARG gid
RUN groupadd --gid ${gid} ${user} && useradd -m --uid ${uid} --gid ${gid} ${user}

RUN apt-mark hold libsystemd0

# ssh (https://docs.docker.com/engine/examples/running_ssh_service/)
RUN apt-get update && apt-get upgrade -y && apt-get install openssh-server -y
RUN mkdir /var/run/sshd
RUN sed -ri 's/^#?PermitRootLogin\s+.*/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -ri 's/UsePAM yes/#UsePAM yes/g' /etc/ssh/sshd_config

### Install ROS Humble
# ensure local
RUN apt-get update && apt-get install locales -y
RUN locale-gen en_US en_US.UTF-8
RUN update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
RUN LANG=en_US.UTF-8

# setup sources
RUN apt-get update && apt-get install curl gnupg lsb-release -y
RUN curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
RUN /bin/bash -c 'echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(source /etc/os-release && echo $UBUNTU_CODENAME) main"' | tee /etc/apt/sources.list.d/ros2.list > /dev/null

# install ROS2 packages
RUN apt-get update && apt-get upgrade -y
RUN apt-mark unhold libsystemd0
RUN apt-get install ros-humble-desktop -y
#
RUN apt-get update && apt-get install python3-rosdep -y && rosdep init && rosdep update
RUN apt-get install python3-colcon-common-extensions -y

#install clang
RUN apt-get install clang-13 lldb-13 lld-13 -y &&\
    ln -s /usr/bin/clang++-13 /usr/bin/clang++ &&\
    ln -s /usr/bin/clang-13 /usr/bin/clang
# install utilities
RUN apt-get install sudo ccls ninja-build -y

ARG display
RUN echo "export DISPLAY=${display}" >> /etc/profile
ENV DISPLAY=${display}
ENV DEBIAN_FRONTEND=newt
