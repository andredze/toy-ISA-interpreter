cmake_minimum_required(VERSION 3.11)

project(toy_ISA_interpreter)

option(ENABLE_SANITIZER_ADDRESS "Enable AddressSanitizer" OFF)

set(EXECUTABLE anki_interpreter)
add_executable(${EXECUTABLE})

set(COMMON_FLAGS -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/= -std=c++20)

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

target_sources(${EXECUTABLE}
    PRIVATE
        src/main.cc
        src/execute.cc
        src/decode.cc
        src/binary_code_input.cc
        src/interpreter.cc
        src/cache.cc

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
)