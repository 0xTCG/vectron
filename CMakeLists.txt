cmake_minimum_required(VERSION 3.14)
project(Vectron)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wno-gnu-zero-variadic-macro-arguments -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-limit-debug-info")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
include_directories(.)

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
    ir/loop.cpp
    ir/loop.h
    ir/vectron.h)
add_library(vectron SHARED ${VECTRON_FILES})

# Include directories for your custom LLVM
target_include_directories(vectron PRIVATE "${CODON_PATH}/include")

# Link directories and libraries for your custom LLVM
target_link_directories(vectron PRIVATE "${CODON_PATH}/lib/codon")

target_link_libraries(vectron PRIVATE codonrt codonc) # LLVMCore LLVMSupport LLVMTarget

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CODON_PATH}/lib/codon" CACHE PATH "Use the existing Codon installation" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

install(TARGETS vectron DESTINATION plugins/vectron/build)
install(FILES plugin.toml DESTINATION plugins/vectron)
install(FILES stdlib/experimental/simd.codon DESTINATION stdlib/experimental)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/stdlib DESTINATION plugins/vectron)
