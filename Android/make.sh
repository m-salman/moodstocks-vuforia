#!/bin/bash

# Get Vuforia SDK relative path

if [[ -z $VUFORIA_SDK_ANDROID ]]; then
  echo "Please provide the path to your version of the Vuforia SDK!"
  echo "Minimum supported version: 2.0.30"
  echo " $ export VUFORIA_SDK_ANDROID=path/to/vuforia-sdk-android-*-*-*/"
  echo " $ bash make.sh"
  exit
fi

source=$(pwd)
target=$VUFORIA_SDK_ANDROID

common_part=$source
result=""

while [[ "${target#$common_part}" == "${target}" ]]; do
    common_part="$(dirname $common_part)"
    if [[ -z $result ]]; then
        result=".."
    else
        result="../$result"
    fi
done

if [[ $common_part == "/" ]]; then
    result="$result/"
fi

forward_part="${target#$common_part}"

if [[ -n $result ]] && [[ -n $forward_part ]]; then
    result="$result$forward_part"
elif [[ -n $forward_part ]]; then
    result="${forward_part:1}"
fi

# Get Moodstocks SDK path

if [[ -z $MOODSTOCKS_SDK_ANDROID ]]; then
  echo "Please provide the path the latest version of the Moodstocks SDK!"
  echo "Minimum supported version: v3.6-r2"
  echo " $ export MOODSTOCKS_SDK_ANDROID=path/to/moodstocks/sdk/"
  echo " $ bash make.sh"
  exit
fi

# Export value used by Android.mk:
export VFR_SDK="../$result"

# Check ndk-build can be found

NDK_BUILD=$(which ndk-build)
if [[ -z ${NDK_BUILD} ]]; then
  if [[ -z $ANDROID_NDK ]]; then
    echo "\`ndk-build\` command not found!"
    echo "Please add it to your path or specify the location of"
    echo "your Android NDK using:"
    echo " $ export ANDROID_NDK=path/to/android/ndk"
    echo " $ bash make.sh"
    exit
  else
    NDK_BUILD=$ANDROID_NDK/ndk-build
  fi
fi

# Build!

$NDK_BUILD clean
$NDK_BUILD

# Copy Moodstocks files, as they were erased by `ndk-build clean`

cp $MOODSTOCKS_SDK_ANDROID/libs/armeabi-v7a/* libs/armeabi-v7a/
cp -R $MOODSTOCKS_SDK_ANDROID/src/* src/
