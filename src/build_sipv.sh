#!/bin/bash

# Adjust this path if your NDK is elsewhere
NDK_PATH=$ANDROID_NDK_HOME
CLANG=$NDK_PATH/toolchains/llvm/prebuilt/linux-x86_64/bin/clang

echo "[*] Compiling 16KB-aligned native core..."
$CLANG -shared -fPIC \
    -Wl,-z,max-page-size=16384 \
    -Wl,-z,common-page-size=16384 \
    -I$NDK_PATH/sysroot/usr/include \
    -o liboracle_core.so oracle_core.c -llog

echo "[*] Baking DEX wrapper..."
javac -d . OracleWrapper.java
d8 --release --output . com/sipv/*.class

echo "[*] Packaging Ghost-Oracle..."
zip -r sipv_unsigned.apk AndroidManifest.xml classes.dex liboracle_core.so
# Assumes you have a keystore ready
# apksigner sign --ks my-key.keystore --out sipv_ready.apk sipv_unsigned.apk

echo "[!] Manual Action: Sign sipv_unsigned.apk and deploy via Shizuku."
