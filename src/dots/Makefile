#! @file
#! @author Vsevolod (Seva) Ivanov
#!

CXX=g++
CXXFLAGS=-std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic \
		-lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -lserialport \
		-I./

GLFW_ARCH=-lglfw
GLFW_LINUX=-lglfw3

CXX_FILES=Window.cpp Shader.cpp Camera.cpp Spinal.cpp Main.cpp

all:
	mkdir -p build

arch-v1: all
	${CXX} ${CXXFLAGS} ${GLFW_ARCH} \
		Window.cpp Shader.cpp Camera.cpp Spinal_v1.cpp \
		-o build/mesh.out

arch: all
	${CXX} ${CXXFLAGS} ${GLFW_ARCH} ${CXX_FILES} \
		-o build/mesh.out

linux: all
	${CXX} ${CXXFLAGS} ${GLFW_LINUX} ${CXX_FILES} \
		-o build/mesh.out

clean:
	rm -rf build/
