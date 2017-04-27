#! @file
#! @author Vsevolod (Seva) Ivanov
#!

CXX=g++
CXXFLAGS=-std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic -lpthread \
		 -lX11 -lXrandr -lXi

GL_LINUX=-lGLEW -lGL

GLFW_ARCH=-lglfw
GLFW_LINUX=-lglfw

EXTRA=-lserialport

INCLUDES=-I./
CXX_FILES=Window.cpp Shader.cpp Camera.cpp Spinal.cpp Main.cpp

all:
	mkdir -p build

linux-v1: all
	${CXX} ${CXXFLAGS} ${GL_LINUX} ${GLFW_LINUX} ${EXTRA} \
		Window.cpp Shader.cpp Camera.cpp Spinal_v1.cpp \
		-o build/mesh.out

arch-v1: all
	${CXX} ${CXXFLAGS} ${GL_LINUX} ${GLFW_ARCH} ${EXTRA} \
		Window.cpp Shader.cpp Camera.cpp Spinal_v1.cpp \
		-o build/mesh.out

pi-v1: all
	${CXX} ${CXXFLAGS} ${GL_LINUX} ${GLFW_ARCH} ${EXTRA} -lpthread \
		Window.cpp Shader.cpp Camera.cpp Spinal_v1.cpp \
		-o build/mesh.out

arch: all
	${CXX} ${CXXFLAGS} ${GL_LINUX} ${GLFW_ARCH} ${EXTRA} \
		${CXX_FILES} -o build/mesh.out

linux: all
	${CXX} ${CXXFLAGS} ${GL_LINUX} ${GLFW_LINUX} ${EXTRA} \
		${CXX_FILES} -o build/mesh.out

clean:
	rm -rf build/
