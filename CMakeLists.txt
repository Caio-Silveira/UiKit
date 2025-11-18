cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(UiKit CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(UIKIT_SOURCES
    src/UI/ui.cpp
    src/UI/ui_context.cpp
    src/UI/ui_shaders.cpp 
)

set(UIKIT_HEADERS
    include/UI/ui.h
    include/UI/ui_context.h
    include/UI/ui_core.h
    include/UI/ui_draw.h
    include/UI/ui_helpers.h
    include/UI/ui_widgets.h
    include/UI/ui_shaders.h
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
)

set_target_properties(UiKit PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)