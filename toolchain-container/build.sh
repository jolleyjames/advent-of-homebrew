#! /bin/bash

if [ -z "$1" ]; then 
  echo Project directory argument missing
  exit 1
fi

cd $1
${DEVKITPRO}/portlibs/wii/bin/powerpc-eabi-cmake -S . -B build
${DEVKITPRO}/portlibs/wii/bin/powerpc-eabi-cmake --build build
${DEVKITPRO}/portlibs/wii/bin/powerpc-eabi-cmake --build build -t zip
