#include <cstdlib>
#include <iostream>
#include "config.hpp"
#include "binary_code_input.hpp"
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

    binary_files_io::BinaryCode bin_code {}; 

    if (!bin_code.ReadFile (argv[1])) {
        return EXIT_FAILURE;
    }

    //==================================================

    return EXIT_SUCCESS;
} 

//--------------------------------------------------------------------------------