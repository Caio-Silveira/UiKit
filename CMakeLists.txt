cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(UiKit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)

set(STATIC_BUILD ON)
if(STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")

    set(BUILD_FOR_WINDOWS TRUE)

    if(BUILD_FOR_WINDOWS)
        set(CMAKE_SYSTEM_NAME Windows)
        set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
        set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
        set(CMAKE_RC_COMPILER /usr/bin/x86_64-w64-mingw32-windres)

        set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

        include_directories(${CMAKE_FIND_ROOT_PATH}/include)
        link_directories(${CMAKE_FIND_ROOT_PATH}/lib)
    endif()
endif()

set(UIKIT_SOURCES
    src/WND.cpp
    src/Input.cpp
    src/UI/tab1.cpp
)

set(UIKIT_HEADERS
    include/DXUT.h
    include/Types.h
    include/WND.h
    include/Input.h
    include/UI/tab1.h
)

add_executable(UiKit WIN32
    main.cpp
    ${UIKIT_SOURCES}
    ${UIKIT_HEADERS}
)

target_include_directories(UiKit PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UiKit PRIVATE
    d3d12
    dxgi
    d3dcompiler
    dxguid
    dwmapi
)

set_target_properties(UiKit PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)