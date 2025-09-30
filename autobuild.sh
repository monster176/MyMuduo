#!/bin/bash

set -e

#没有=>创建
if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*

cd `pwd`/build &&
    cmake .. &&
    make

# 回到上级目录
cd ..

# 把头文件/so库安装到相应的位置
# /usr/include/mymuduo   so=> /usr/lib

if [ ! -d /usr/include/mymuduo ]; then
    mkdir /usr/include/mymuduo
fi

for hander in `ls *.h`

do
    cp $hander /usr/include/mymuduo
done

cp `pwd`/lib/libMyMuduo.so /usr/lib

ldconfig

