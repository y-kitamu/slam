#!/bin/bash

echo "TARGET_USER = ${TARGET_USER}"
sudo -u ${TARGET_USER} "/home/${TARGET_USER}/dotfiles/install.sh"
/sbin/sshd -D
