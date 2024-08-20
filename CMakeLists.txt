cmake_minimum_required(VERSION 3.14)
project(Vectron)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wno-return-type-c-linkage")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-limit-debug-info")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
include_directories(.)

set(CPM_DOWNLOAD_VERSION 0.32.3)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake...")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
    NAME xz
    GITHUB_REPOSITORY "xz-mirror/xz"
    VERSION 5.2.5
    GIT_TAG e7da44d5151e21f153925781ad29334ae0786101
    EXCLUDE_FROM_ALL YES
    OPTIONS "BUILD_SHARED_LIBS OFF"
            "CMAKE_POSITION_INDEPENDENT_CODE ON")
if(xz_ADDED)
    set_target_properties(xz PROPERTIES EXCLUDE_FROM_ALL ON)
    set_target_properties(xzdec PROPERTIES EXCLUDE_FROM_ALL ON)
endif()

set(VECTRON_FILES
    ir/bypass.cpp
    ir/bypass.h
    ir/func_repl.cpp
    ir/func_repl.h
    ir/loop_anlz.cpp
    ir/loop_anlz.h
    ir/list_init.cpp
    ir/list_init.h
    ir/env_select.cpp
    ir/env_select.h
    ir/vectron.cpp
    ir/vectron.h)
add_library(vectron SHARED ${VECTRON_FILES})

# Include directories for your custom LLVM
target_include_directories(vectron PRIVATE
    "${CODON_PATH}/include"
    "${LLVM_PATH}/include")

# Link directories and libraries for your custom LLVM
target_link_directories(vectron PRIVATE
    "${CODON_PATH}/lib/codon"
    "${LLVM_PATH}/lib")

target_link_libraries(vectron PRIVATE codonrt codonc LLVMCore LLVMSupport LLVMTarget)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CODON_PATH}/lib/codon" CACHE PATH "Use the existing Codon installation" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

install(TARGETS vectron DESTINATION plugins/vectron/build)
install(FILES plugin.toml DESTINATION plugins/vectron)
install(FILES stdlib/experimental/simd.codon DESTINATION stdlib/experimental)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/stdlib DESTINATION plugins/vectron)

