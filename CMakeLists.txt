cmake_minimum_required(VERSION 2.6)
project(i2gmm)

macro(use_cxx11)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_CXX_FLAGS "--std=gnu++11 -fpermissive -O3 ${CMAKE_CXX_FLAGS}")
        endif ()
    else ()
        set (CMAKE_CXX_STANDARD 11)
    endif ()
endmacro(use_cxx11)


set(CMAKE_CXX_STANDARD 11)
use_cxx11()


set(SOURCE_FILES
    src/Customer.cpp
    src/Dish.cpp
    src/i2gmm.cpp
    src/Restaurant.cpp
    src/Table.cpp
    src/Customer.h
    src/Dish.h
    src/Restaurant.h
    src/Table.h)

INCLUDE_DIRECTORIES(../GMMBase/)
LINK_DIRECTORIES(../GMMBase)
INCLUDE_DIRECTORIES(../FastMat/)
LINK_DIRECTORIES(../FastMat)
find_package (Threads)
add_executable(i2gmm ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(i2gmm FastMat)
TARGET_LINK_LIBRARIES (i2gmm ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(i2gmm GMMBase)
