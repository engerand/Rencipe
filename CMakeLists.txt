#
# Ren'cipe 3DS Engine
#

cmake_minimum_required(VERSION 3.19.4)
project(rencipe)

# Enable ASM/C/C++ compilation (needed for 3DS)
enable_language(ASM C CXX)

# Use DevkitARM suggested flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DKA_SUGGESTED_CXX_FLAGS}")

# Ensure C++ exceptions are enabled for compilers that require an explicit flag
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # -fexceptions enables proper exception handling (needed for some libraries / std usage)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
endif()

# Add DevkitPro cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake-support)
include(Tools3DS)

# Find libraries
find_package(CTRULIB REQUIRED)
find_package(CITRO3D REQUIRED)
find_package(CITRO2D REQUIRED)

# Collect source files (.cpp / .c only)
file(GLOB_RECURSE SOURCE_FILES
    source/*.cpp
    source/*.c
)

# Create executable
add_executable(rencipe ${SOURCE_FILES})

# Ensure ROMFS is included in .3dsx
if (DEFINED _3DSXTOOL AND _3DSXTOOL)
    add_3dsx_target(rencipe
        ROMFS ${CMAKE_SOURCE_DIR}/romfs
    )
else()
    message(WARNING "3dsxtool not found; skipping .3dsx generation. Setting LINK_FLAGS to use 3dsx specs for local build.")
    set_target_properties(rencipe PROPERTIES LINK_FLAGS "-specs=3dsx.specs")
endif()


# Include DevkitPro / Citro2D headers
target_include_directories(rencipe PRIVATE
    ${CITRO2D_INCLUDE_DIRS}
    ${CITRO3D_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(rencipe
    3ds::citro2d
    3ds::citro3d
    3ds::ctrulib
    m
)

# Set C++ standard
set_property(TARGET rencipe PROPERTY CXX_STANDARD 17)

# # Generate 3DSX target
# if(DEFINED _3DSXTOOL AND _3DSXTOOL)
#     add_3dsx_target(rencipe)
# else()
#     message(WARNING "3dsxtool not found; skipping .3dsx generation. Setting LINK_FLAGS to use 3dsx specs for local build.")
#     # Ensure the executable is linked with the 3dsx specs so it matches expected build flags
#     set_target_properties(rencipe PROPERTIES LINK_FLAGS "-specs=3dsx.specs")
# endif()

