#!/bin/bash
CPU=x86
TOOLCHAIN=$NDK_ROOT/toolchains/$CPU-4.9/prebuilt/windows-x86_64
#export TMPDIR="D:/FFmpeg/temp"
#从as的 externalNativeBuild/xxx/build.ninja
FLAGS="-isystem $NDK_ROOT/sysroot/usr/include/i686-linux-android -D__ANDROID_API__=21 -g -DANDROID -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -mstackrealign -Wa,--noexecstack -Wformat -Werror=format-security  -O0 -fPIC"
INCLUDES="-isystem $NDK_ROOT/sources/android/support/include"

PREFIX=./android/x86_lsn11

./configure \
--prefix=$PREFIX \
--enable-small \
--disable-programs \
--disable-avdevice \
--disable-encoders \
--disable-muxers \
--disable-filters \
--enable-cross-compile \
--cross-prefix=$TOOLCHAIN/bin/i686-linux-android- \
--disable-shared \
--enable-static \
--sysroot=$NDK_ROOT/platforms/android-21/arch-x86 \
--extra-cflags="$FLAGS $INCLUDES" \
--extra-cflags="-isysroot $NDK_ROOT/sysroot/" \
--arch=x86 \
--target-os=android 

# 清理一下 
make clean
#执行makefile
make install
