cmake_minimum_required(VERSION 3.0.0)
project(spica
	LANGUAGES   C
	VERSION     0.0.1
	DESCRIPTION "Astrophotography image processing library")
include(GNUInstallDirs)

set(EXTRA_C_FLAGS "-Wall -pedantic -std=c11 -O2")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_C_FLAGS}")

file(GLOB SOURCE_FILES
	src/*.c
	src/SCoarseAlign/*.c
	src/SImage/*.c
	src/SImageSource/*.c
	src/SSequence/*.c
	src/SStar/*.c
	src/SStarMatcher/*.c)
file(GLOB INCLUDE_FILES
	include/*.h)

add_library(spica STATIC ${SOURCE_FILES})

target_include_directories(spica PRIVATE include src)
target_include_directories(spica PUBLIC include)

install(FILES ${INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/spica)
install(TARGETS spica
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBRID})

add_subdirectory(examples EXCLUDE_FROM_ALL)
