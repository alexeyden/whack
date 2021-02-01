#!/bin/bash

NDK_ROOT="/home/rtgbnm/Devel/Code/Active/Android/NDK"

if [[ $1 == "android" ]]; then
    mkdir android
    mkdir android/SDL2
    mkdir android/jsoncpp

		cd android/jsoncpp
    
    cmake ../../jsoncpp \
    -DJSONCPP_WITH_TESTS=OFF \
    -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=19 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_NDK=$NDK_ROOT \
    -DCMAKE_ANDROID_STL_TYPE=gnustl_static
    make -j4
    
		cd ../../android/SDL2

    /home/rtgbnm/Devel/Code/Active/Android/NDK/ndk-build \
			NDK_PROJECT_PATH=../../SDL2 \
			APP_BUILD_SCRIPT=../../SDL2/Android_static.mk \
			NDK_MODULE_PATH=../../ \
			APP_PLATFORM=android-19 \
			APP_ABI="armeabi-v7a x86"
    
elif [[ $1 == "linux" ]]; then
    mkdir linux
    mkdir linux/SDL2
    mkdir linux/jsoncpp
    
    cd linux/SDL2
    cmake ../../SDL2 \
    -DSDL_STATIC=ON \
    -DSDL_SHARED=OFF \
    -DSNDIO=OFF
    make -j4
    
    cd ../jsoncpp
    cmake ../../jsoncpp \
    -DJSONCPP_WITH_TESTS=OFF \
    -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF
    make -j4
elif [[ $1 == "clean" ]]; then
    rm -Rf linux
    rm -Rf android
else
    echo "wrong option"
fi
