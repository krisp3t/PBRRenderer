cmake_minimum_required(VERSION 3.12)
project(PBRRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} WIN32
        include/dxerr/dxerr.cpp
        src/WinMain.cpp
        src/Window.cpp
        src/App.cpp
        src/Graphics.cpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE=1 _UNICODE=1)
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(${PROJECT_NAME} PRIVATE d3d11)
