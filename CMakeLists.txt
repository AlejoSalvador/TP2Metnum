cmake_minimum_required(VERSION 3.7)
project(trabajoPractico2METNUM)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O3")

set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")

set(SOURCE_FILES main.cpp moduloPCA.h moduloPCA.cpp cargadorDeImagenes.cpp cargadorDeImagenes.h lib/ppmloader.h lib/ppmloader.cpp knn.hpp kfold.hpp)
add_executable(trabajoPractico2METNUM ${SOURCE_FILES})