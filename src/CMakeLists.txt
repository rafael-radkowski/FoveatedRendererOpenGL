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
# project(FoveatedRendererOpenGL)


# Find packages
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glew REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(GLFW3 REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen3)

if (Eigen3_FOUND)
message(STATUS "Found Eigen3 at ${EIGEN3_INCLUDE_DIR}")
else()
message(STATUS "Did not find Eigen3")
endif()


if (GLEW_FOUND)
message(STATUS "Found GLEW ${GLEW_INCLUDE_DIR} and ${GLEW_LIBRARIES}")
endif()

if (OPENGL_FOUND)
message(STATUS "Found OPENGL ${OPENGL_INCLUDE_DIR} and ${OPENGL_LIBRARIES}")
endif()

if (GLFW_FOUND)
message(STATUS "Found GLEW ${GLFW_INCLUDE_DIRS} and ${GLFW3_LIBRARY}")
else()
IF(WIN32)
ELSE(WIN32)
include_directories("/usr/local/Cellar/glfw/3.2.1/include")
include_directories("/usr/local/Cellar/glfw/1.11.0/include")
set( GLFW3_LIBRARY glfw)
ENDIF(WIN32)
endif()

# Include dirs
include_directories(../external/gl_common)
include_directories(./../../external/gl_common)
include_directories(../external/gl_ext)
include_directories(./../../external/gl_ext)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIR})

# Add all files to the configuration
file(GLOB FR_SRC
	main.cpp
	Exists.h
	Exists.cpp
	EyeData.h
	EyeData.cpp
	EyeDataReader.cpp
	EyeDataReader.h
	TimeUtils.h
	TimeUtils.cpp
	Types.h
	EyeRenderHelpers.h
	EyeRenderHelpers.cpp
	ArgParser.h
	ArgParser.cpp	
	FoveatedRendererBase.h
	FoveatedRendererBase.cpp
	FoveatedRenderingScene.h
	FoveatedRenderingScene.cpp
	main_scene.cpp
	DefaultModels.h
	DefaultModels.cpp
	TimeMeasurement.h
	TimeMeasurement.cpp


)

file(GLOB GL_SRC
	../external/gl_common/OpenGLDefaults.h
	../external/gl_common/OpenGLDefaults.cpp
	../external/gl_common/VertexBuffers.h
	../external/gl_common/VertexBuffers.cpp
	../external/gl_common/ShaderProgram.h
	../external/gl_common/ShaderProgram.cpp
	../external/gl_common/TrackballControls.h
	../external/gl_common/TrackballControls.cpp
	../external/gl_common/ModelCoordinateSystem.cpp
	../external/gl_common/ModelCoordinateSystem.h
	../external/gl_common/Window.cpp
	../external/gl_common/Window.h
	../external/gl_common/ModelPlane.h
	../external/gl_common/ModelPlane.cpp
    ../external/gl_common/Texture2D.h
	../external/gl_common/Texture2D.cpp
	../external/gl_common/BMPLoader.h
	../external/gl_common/BMPLoader.cpp
	../external/gl_common/ModelOBJ.h
	../external/gl_common/ModelOBJ.cpp
	../external/gl_common/ModelOBJExt.h
	../external/gl_common/ModelOBJExt.cpp
	../external/gl_common/ModelSphere.h
	../external/gl_common/ModelSphere.cpp
	../external/gl_common/RenderToTexture.h
	../external/gl_common/CameraControls.h
	../external/gl_common/CameraControls.cpp
	../external/gl_common/CommonTypes.h
	../external/gl_common/FileUtils.h
	../external/gl_common/FileUtils.cpp
	../external/gl_ext/TextureLoader.h
	../external/gl_ext/TextureLoader.cpp
	../external/gl_ext/MaterialLibrary.h
	../external/gl_ext/MaterialLibrary.cpp
)


source_group( src FILES ${FR_SRC})
source_group( graphics FILES ${GL_SRC})


add_compile_definitions(_WITH_CAMERA)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)



link_directories("C:/SDK/lib")

# Create an executable
add_executable(${PROJECT_NAME} ${FR_SRC} ${GL_SRC} )


# Add link directories
# Note required for this project
IF(WIN32)
	link_directories(${CMAKE_SOURCE_DIR}/../SDK/lib)
ELSE(WIN32)
	link_directories(${CMAKE_SOURCE_DIR}/../SDK/lib)
	link_directories(/usr/local/lib)
	link_directories(/usr/local/Cellar/glfw3/3.0.4/lib)
	link_directories(/usr/local/Cellar/glfw3/3.0.4/lib)
	link_directories(/usr/local/Cellar/glew/1.11.0/lib)
	link_directories(/usr/local/Cellar/glfw/3.2.1/lib)
ENDIF(WIN32)

message(${GLEW_LIBS})

# Add libraries
target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS} ${GLEW_LIBS} ${GLEW_LIBRARY} ${GLFW3_LIBS} ${OPENGL_LIBRARIES} ${OPENGL_LIBRARIES} )
