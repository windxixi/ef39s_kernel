#!/bin/bash

export ARCH=arm
export CROSS_COMPILE=../../../../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-

make -j4

