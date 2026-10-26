#include <cstdlib>
#include "elf_loader.hpp"
#include "interpreter.hpp"
#include "cpu.hpp"
#include "error_handle.hpp"
#include "logger.hpp"

//--------------------------------------------------------------------------------

int main (int argc, char** argv)
{
    if (argc != 2)
    {
        error_handle::PrintError (
            "Wrong arguments\n"
            "Usage: {} <elf_file>", 
            toy_isa_interpreter::kExecutableFileName);

        return EXIT_SUCCESS;
    }

    LOG_TRACE_("Entered interpreter");

    //==================================================

    std::string file_path = argv[1];

    return toy_isa_interpreter::ExecuteProgram (file_path);
} 

//--------------------------------------------------------------------------------