#!/bin/bash
insmod char_dev.ko version=6,17 time=10
cat /dev/char_dev
whoami > /dev/char_dev
rmmod /dev/char_dev
dmesg | tail -n 7