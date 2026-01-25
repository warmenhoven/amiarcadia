
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("droidarcadia")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             droidarcadia

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
        android.c
        2650.c
        arcadia.c
        coin-ops.c
        elektor.c
        formats.c
        interton.c
        malzak.c
        pvi.c
        zaccaria.c
        rc_compat.c
        rc_client.c
        rc_util.c
        rapi/rc_api_common.c
        rapi/rc_api_info.c
        rapi/rc_api_runtime.c
        rapi/rc_api_user.c
        rcheevos/alloc.c
        rcheevos/condition.c
        rcheevos/condset.c
        rcheevos/consoleinfo.c
        rcheevos/format.c
        rcheevos/lapi.c
        rcheevos/lauxlib.c
        rcheevos/lboard.c
        rcheevos/lcode.c
        rcheevos/lctype.c
        rcheevos/ldebug.c
        rcheevos/ldo.c
        rcheevos/ldump.c
        rcheevos/lfunc.c
        rcheevos/lgc.c
        rcheevos/llex.c
        rcheevos/lmem.c
        rcheevos/lobject.c
        rcheevos/lopcodes.c
        rcheevos/lparser.c
        rcheevos/lstate.c
        rcheevos/lstring.c
        rcheevos/ltable.c
        rcheevos/ltm.c
        rcheevos/luac.c
        rcheevos/lundump.c
        rcheevos/lvm.c
        rcheevos/lzio.c
        rcheevos/memref.c
        rcheevos/operand.c
        rcheevos/rc_validate.c
        rcheevos/richpresence.c
        rcheevos/runtime.c
        rcheevos/runtime_progress.c
        rcheevos/trigger.c
        rcheevos/value.c
        rhash/aes.c
        rhash/hash.c
        rhash/md5.c
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       droidarcadia

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=excess-initializers")

add_definitions(-Wno-invalid-source-encoding)
