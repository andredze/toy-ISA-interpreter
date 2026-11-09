cmake_minimum_required(VERSION 3.28)

project(toy_ISA_interpreter)

set(EXECUTABLE interpreter)
add_executable(${EXECUTABLE})

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(IR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ir")
set(IR_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/ir")

add_custom_target(build_ir
    COMMAND mkdir -p "${IR_BUILD_DIR}"
    COMMAND make -C "${IR_BUILD_DIR}"
        -f "${IR_SOURCE_DIR}/Makefile"
        "CFLAGS=-w"
        "BUILD_CFLAGS=-w"
        "MINILUA_CFLAGS=-O2 -w"
        "SRC_DIR=${IR_SOURCE_DIR}"
        "BUILD_DIR=${IR_BUILD_DIR}"
        "${IR_BUILD_DIR}/libir.a"
    BYPRODUCTS "${IR_BUILD_DIR}/libir.a"
    JOB_SERVER_AWARE TRUE
    VERBATIM
)

add_dependencies(${EXECUTABLE} build_ir)

target_include_directories(${EXECUTABLE} SYSTEM PRIVATE
    "${IR_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/elfio"
)

target_link_libraries(${EXECUTABLE} PRIVATE
    "${IR_BUILD_DIR}/libir.a"
    capstone
    m
    ${CMAKE_DL_LIBS}
)

target_compile_features(${EXECUTABLE} PRIVATE cxx_std_20)

set(COMMON_FLAGS -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/=)

set(RELEASE_FLAGS -DNDEBUG -O3 -march=native -pie -fPIE -flto)

set(ASAN -fstack-protector -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

set(DEBUG_FLAGS -O0 "-D LOGGING" "-D _DEBUG" -ggdb3 -fstrict-overflow -flto-odr-type-merging
			    -fno-omit-frame-pointer -pie -fPIE -fcheck-new -fsized-deallocation
)

set(WARNINGS -Wall -Wextra -Weffc++ -Wsign-conversion -Waggressive-loop-optimizations
             -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual
             -Wchar-subscripts -Wconditionally-supported -Wctor-dtor-privacy
             -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security
             -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor
             -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self
             -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel
             -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods
             -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum
             -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast
             -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers
             -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector
             -Wlarger-than=8192 -Wstack-usage=8192 -Werror=vla -Wconversion
)

if (DEBUG_MODE)
    target_compile_options(${EXECUTABLE}
        PRIVATE
            ${COMMON_FLAGS}
            ${ASAN}
            ${DEBUG_FLAGS}
            ${WARNINGS}
    )
    target_link_options(${EXECUTABLE}
        PRIVATE
            ${COMMON_FLAGS}
            ${ASAN}
            ${DEBUG_FLAGS}
            ${WARNINGS}
    )
else()
    target_compile_options(${EXECUTABLE}
        PRIVATE
            ${COMMON_FLAGS}
            ${RELEASE_FLAGS}
    )
    target_link_options(${EXECUTABLE}
        PRIVATE
            ${COMMON_FLAGS}
            ${RELEASE_FLAGS}
    )
endif()

set (CXX_SRC
        src/execute.cc
        src/decode.cc
        src/binary_code_input.cc
        src/interpreter.cc
        src/bb_cache.cc
        src/bt_cache.cc
        src/elf_loader.cc
        src/translator.cc
)

if (TESTS)
    target_sources(${EXECUTABLE}
        PRIVATE
            tests/main.cc
            ${CXX_SRC}

        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS
                tests
                include
    )
    target_link_libraries(${EXECUTABLE} gtest gmock pthread)
else()
    target_sources(${EXECUTABLE}
        PRIVATE
            ${CXX_SRC}
            src/main.cc

        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS
                include
    )
endif()
