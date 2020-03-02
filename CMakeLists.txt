# Main cmake file 
# Foveated Rendering Shader Test 
# Rafael Radkowski
# Iowa State University
# rafael@iastate.edu
#
# Nov. 12, 2018


cmake_minimum_required(VERSION 2.6)

# policy for relative subfolders
cmake_policy(SET CMP0015 NEW)
cmake_policy(SET CMP0074 NEW)



# Main project name
project(FoveatedRendererOpenGL)


set(OUTPUT_BINDIR ${CMAKE_SOURCE_DIR}/bin)
make_directory(${OUTPUT_BINDIR})
make_directory(${OUTPUT_BINDIR}/logs)

# Set the project architecture for this example
set( PROJECT_ARCH "x64" )

# All output files are copied to bin
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Set the runtime output path
set( RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")


SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OutputDir})
set(CMAKE_DEBUG_POSTFIX d)


set(CFILE_PATH  "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/" ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

message(STATUS "Source directory ${CMAKE_SOURCE_DIR}")
#message(STATUS "SDK directory ${CMAKE_SOURCE_DIR}/../SDK")


add_subdirectory(src)