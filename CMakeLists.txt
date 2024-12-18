cmake_minimum_required(VERSION 3.29)
project(Proyect1 C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "D:/7mo/CompGraph/SDL2-2.30.7-mingw/SDL2-devel-2.30.7-mingw/SDL2-2.30.7/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS})


add_executable(Proyect1 main.c
        mesh.c
        triangle.c
        array.c
        vector.c
        display.c
        matrix.c
        render.h
        render.c
        transform.c
        transform.h
        main.c)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})

