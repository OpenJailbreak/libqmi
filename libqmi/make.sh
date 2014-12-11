#!/bin/bash
prefix=""
if test -d "/Applications/Xcode.app/Contents/Developer"; then
	prefix="/Applications/Xcode.app/Contents"
fi

sysroot="${prefix}/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
if test -d "${prefix}/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.1.sdk"; then
	sysroot="${prefix}/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.1.sdk"
fi

CC="/Developer/Platforms/iPhoneOS.platform/Developer/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-g++-4.2 -arch armv7 -isysroot $sysroot -framework CoreFoundation -framework Foundation -framework IOKit"

${CC} QMIControlPacket.cpp QMIPacket.cpp QMIUSBController.cpp dloadtool.cpp etl.cpp usb.cpp QMIController.cpp QMISPIController.cpp QMux.cpp QMIClient.cpp QMux.h dbltool.cpp main.c util.cpp QMIClient.h QMIFuzzer.cpp QMIService.cpp TLV.cpp diag.cpp QMIConsole.cpp QMILibController.cpp dload.cpp qcommtool.cpp -o libqmi.dylib -shared
mv libqmi.dylib ../../vpnplugin/Staging/libqmi.dylib
