set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(triple armv7m-unknown-linux-gnueabihf)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=lld")
set(CMAKE_STATIC_LINKER_FLAGS "-fuse-ld=lld")
set(CMAKE_EXE_LINKER_FLAGS  "-fuse-ld=lld")
add_definitions("-mcpu=cortex-a53 -mfpu=neon-vfpv4 -mfloat-abi=hard -marm")
# rsync that comes with macOS Sierra 10.12 does not support multiple remote sources. Using the rsync installed via brew install rsync however does.
# mkdir sysroot
# rsync -rzLR --safe-links \
#       pi@raspberrypi:/usr/lib/arm-linux-gnueabihf \
#       pi@raspberrypi:/usr/lib/gcc/arm-linux-gnueabihf \
#       pi@raspberrypi:/usr/include \
#       pi@raspberrypi:/lib/arm-linux-gnueabihf \
#       sysroot/

# export LDFLAGS=-fuse-ld=lld
# cmake .. -GNinja -DCMAKE_TOOLCHAIN_FILE=... -DCMAKE_SYSROOT=...
