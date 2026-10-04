#include <cstdlib>
#include <iostream>
#include "interpreter.hpp"
#include "cpu.hpp"
#include "error_handle.hpp"
#include "logger.hpp"

//--------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        error_handle::PrintError (
            "Error: you must provide an input file\n"
            "Usage: {} <binary_filename>", 
            toy_isa_interpreter::kExecutableFileName);

        return EXIT_SUCCESS;
    }

    LOG_TRACE_("Entered interpreter");

    //==================================================

    toy_isa_interpreter::BinaryCode bin_code {}; 

    if (!bin_code.ReadFile (argv[1])) {
        return EXIT_FAILURE;
    }

    LOG_TRACE_("Read file {}", argv[1]);

    //==================================================

    return toy_isa_interpreter::ExecuteProgram (bin_code);
} 

//--------------------------------------------------------------------------------