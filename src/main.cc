#include <cstdlib>
#include <iostream>
#include "toy_interpreter.hpp"
#include "binary_code_input.hpp"
#include "error_handle.hpp"

//--------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        error_handle::PrintError (
            "Error: you must provide an input file\n"
            "Usage: {} <binary_filename>", 
            toy_interpreter::kExecutableFileName);

        return EXIT_SUCCESS;
    }

    //==================================================

    binary_files_io::BinaryCode bin_code {}; 

    if (!bin_code.ReadFile (argv[1])) {
        return EXIT_FAILURE;
    }

    //==================================================

    return EXIT_SUCCESS;
} 

//--------------------------------------------------------------------------------