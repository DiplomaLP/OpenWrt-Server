cmake_minimum_required(VERSION 3.5)
project(OpenWrt_Server)

SET(CMAKE_C_COMPILER /home/yura/open_wrt/openwrt/staging_dir/toolchain-mips_24kc_gcc-5.5.0_musl/bin/mips-openwrt-linux-gcc)

SET(CMAKE_CXX_COMPILER /home/yura/open_wrt/openwrt/staging_dir/toolchain-mips_24kc_gcc-5.5.0_musl/bin/mips-openwrt-linux-g++)

set (ENV{STAGING_DIR} ~/open_wrt/openwrt/staging_dir/toolchain-mips_24kc_gcc-5.5.0_musl)
set( ENV{PATH} ~/open_wrt/openwrt/staging_dir/toolchain-mips_24kc_gcc-5.5.0_musl/bin)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c)
add_executable(OpenWrt_Server ${SOURCE_FILES})