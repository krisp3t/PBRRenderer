cmake_minimum_required(VERSION 3.12)
project(PBRRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_BINARY_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

if (NOT DEFINED FXC)
    find_program(FXC fxc.exe REQUIRED)
    if (NOT FXC)
        message(FATAL_ERROR "FXC (fxc.exe) shader compiler not found. Set FXC manually.")
    endif ()
endif ()

set(SHADER_FILES
        VertexShader.hlsl
        PixelShader.hlsl
)
foreach (SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    if (${SHADER_NAME} MATCHES "Vertex")
        set(SHADER_TYPE "vs")
    elseif (${SHADER_NAME} MATCHES "Pixel")
        set(SHADER_TYPE "ps")
    else ()
        message(FATAL_ERROR "Unknown shader type in ${SHADER_NAME}")
    endif ()

    set(SHADER_INPUT ${SHADER_SOURCE_DIR}/${SHADER})
    set(SHADER_OUTPUT ${SHADER_BINARY_DIR}/${SHADER_NAME}.cso)

    add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            COMMAND ${FXC} /nologo /E main /T ${SHADER_TYPE}_5_0 /Fo ${SHADER_OUTPUT} ${SHADER_INPUT}
            DEPENDS ${SHADER_INPUT}
            COMMENT "Compiling shader: ${SHADER}"
    )
    list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
endforeach ()

add_executable(${PROJECT_NAME} WIN32
        include/dxerr/dxerr.cpp
        src/WinMain.cpp
        src/Window.cpp
        src/App.cpp
        src/Graphics.cpp
)

add_custom_target(shaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(${PROJECT_NAME} shaders)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE=1 _UNICODE=1)
target_link_libraries(${PROJECT_NAME} PRIVATE d3d11 D3DCompiler)
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)