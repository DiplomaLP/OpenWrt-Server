#!/bin/sh

cd build/Debug/

STAGING_DIR=~/open_wrt/openwrt/staging_dir/toolchain-mips_24kc_gcc-5.5.0_musl

PATH=$PATH:$STAGING_DIR/bin

$STAGING_DIR/bin/mips-openwrt-linux-gcc ../../main.c ../../server.c ../../server_subscribers.c -o server -static

ret_code=$?
if [ $ret_code != 0 ]; then
  printf "Error : [%d] when executing command: 'gcc'" $ret_code
  exit $ret_code
else
  printf "The program compiled successfully\n"
fi

scp server root@GL-AR150:/root/test/Server/server

ret_code=$?
if [ $ret_code != 0 ]; then
  printf "Error : [%d] when executing command: 'scp'" $ret_code
  exit $ret_code
else
  printf "The program transferred successfully\n"
fi
