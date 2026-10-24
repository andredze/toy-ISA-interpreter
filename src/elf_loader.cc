#include "elf_loader.hpp"
#include "elfio/elf_types.hpp"
#include "error_handle.hpp"
#include "logger.hpp"

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

namespace ELF 
{

using error_handle::PrintError;

//--------------------------------------------------------------------------------

static bool ValidateHeader (const ELFIO::elfio& reader)
{
    if (reader.get_class () != kClass) {
        PrintError ("Can only process 32-bit ELF file");
        return false;
    }

    if (reader.get_encoding () != kEncoding) {
        PrintError ("Can only process little-endian ELF file");
        return false;
    }

    if (reader.get_os_abi () != kOsAbi) {
        PrintError ("Can only process ELF file with toy ISA ABI with code {}", 
                    kOsAbi);
        return false;
    }

    if (reader.get_abi_version () != kAbiVersion) {
        PrintError ("Can only process little-endian ELF file");
        return false;
    }

    if (reader.get_type () != ELFIO::ET_EXEC) {
        PrintError ("Can only process EV_CURRENT version ELF file");
        return false;
    }

    if (reader.get_machine () != kMachineCode) {
        PrintError ("Can only process toy ISA ELF files with machine code {}", 
                    kMachineCode);
        return false;
    }

    if (reader.get_elf_version () != ELFIO::EV_CURRENT) {
        PrintError ("Can only process EV_CURRENT version ELF file");
        return false;
    }

    if (reader.get_version () != ELFIO::EV_CURRENT) {
        PrintError ("Can only process EV_CURRENT version ELF file");
        return false;
    }

    LOG_TRACE_(
        "ELF file header:\n"
        "Class {}\n"
        "Encoding {}\n"
        "OS_ABI {}\n"
        "ABI version {}\n"
        "Type {}\n"
        "Machine {}\n"
        "ELF version {}\n"
        "Version {}\n",
        reader.get_class (),
        reader.get_encoding (),
        reader.get_os_abi (),
        reader.get_abi_version (),
        reader.get_type (),
        reader.get_machine (),
        reader.get_elf_version (),
        reader.get_version ()
    );

    return true;
}

//--------------------------------------------------------------------------------

bool LoadFile (const std::string& file_path)
{
    ELFIO::elfio reader {};

    if (!reader.load (file_path)) {
        PrintError ("Can't find or process an ELF file {}", file_path);
        return false;
    }

    if (!ValidateHeader (reader)) {
        return false;
    }

    ELFIO::Elf_Half sections_count = reader.sections.size ();

    if (sections_count != 0) {
        std::cerr << "[WARNING] Interpreter can't load sections, "
                  << "however given ELF file " << file_path 
                  << "contains " << sections_count << "sections\n";
    }

    ELFIO::Elf_Half segments_count = reader.segments.size ();

    if (segments_count != 1) {
        PrintError ("Interpreter can only load ELF files with 1 segment");
        return false;
    }

    const ELFIO::segment* segment = reader.segments[0];

    return true;
}

//--------------------------------------------------------------------------------

}; // namespace ELF

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————