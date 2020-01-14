cmake_minimum_required(VERSION 3.0)


project(lsp LANGUAGES C)

#enable_testing()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# only build examples if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL "lsp")
  option(LSP_BUILD_EXAMPLES "Build lsp examples" ON)
endif()

set(C_FLAGS
 -g
 -D_GNU_SOURCE
 -Wall
 -Wextra
 -Werror
 -Wno-unused-parameter
 -std=c99
 )
if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND C_FLAGS "-m32")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  list(APPEND C_FLAGS "-Wno-null-dereference")
  list(APPEND C_FLAGS "-Wno-sign-conversion")
  list(APPEND C_FLAGS "-Wno-unused-local-typedef")
  list(APPEND C_FLAGS "-Wthread-safety")
endif()
string(REPLACE ";" " " CMAKE_C_FLAGS "${C_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "-O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_library(LIB_PTHREAD names pthread)
#find_package(Protobuf)
#find_package(ZLIB)



#find_path(CARES_INCLUDE_DIR ares.h)
#find_library(CARES_LIBRARY NAMES cares)
#find_program(THRIFT_COMPILER thrift)
#find_library(HIREDIS_LIBRARY NAMES hiredis)

if(LIB_PTHREAD_FOUND)
  message(STATUS "found libpthread")
endif()

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_DIR}/include)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "C_FLAGS = " ${CMAKE_C_FLAGS} " " ${CMAKE_C_FLAGS_${BUILD_TYPE}})

add_subdirectory(src/datetime/)
add_subdirectory(src/logs)

if(LSP_BUILD_EXAMPLES)
#  add_subdirectory(contrib)
#  add_subdirectory(examples)
endif()

#add_executable(lsp main.c)
#install(TARGETS lsp RUNTIME DESTINATION bin)








