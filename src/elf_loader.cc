#include "elf_loader.hpp"
#include "memory.hpp"
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

    auto abi = reader.get_os_abi ();

    if (abi != kOsAbi) {
        PrintError ("Can only process ELF file with toy ISA ABI with code {}, got {}", 
                    kOsAbi, abi);
        return false;
    }

    if (reader.get_abi_version () != kAbiVersion) {
        PrintError ("Can only process ELF file with abi version {}", kAbiVersion);
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

void ValidateSections (const ELFIO::elfio& reader)
{
    ELFIO::Elf_Half sections_count = reader.sections.size ();

    if (sections_count != 0) {
        std::cerr << "[WARNING] Interpreter can't load sections, "
                  << "however given ELF file contains " 
                  << sections_count << "sections\n";
    }
}

//--------------------------------------------------------------------------------

bool LoadFile (const std::string& file_path, RAM& ram, Word* entry_dest)
{
    assert (entry_dest != nullptr);

    ELFIO::elfio reader {};

    if (!reader.load (file_path)) {
        PrintError ("Can't find or process an ELF file {}", file_path);
        return false;
    }

    if (!ValidateHeader (reader)) {
        return false;
    }

    ValidateSections (reader);

    ELFIO::Elf_Half segments_count = reader.segments.size ();

    if (segments_count != 1) {
        PrintError ("Interpreter can only load ELF files with 1 segment");
        return false;
    }

    const ELFIO::segment* segment = reader.segments[0];

    if (segment->get_type () != ELFIO::PT_LOAD) {
        PrintError ("Interpreter can only load segments with PT_LOAD type");
        return false;
    }

    if (segment->get_flags () != (ELFIO::PF_R | ELFIO::PF_X)) {
        PrintError ("ELF file segment does not have permission "
                    "for reading and executing PF_R | PF_X");
        return false;
    }

    const ELFIO::Elf_Xword file_size = segment->get_file_size ();
    const ELFIO::Elf_Xword mem_size  = segment->get_memory_size ();

    if (file_size > mem_size) {
        PrintError ("ELF file segment 0 has file_size > memory_size");
        return false;
    }
    
    const ELFIO::Elf64_Addr address = segment->get_virtual_address ();

    const std::size_t ram_cap = ram.GetCapacity ();

    if (address > ram_cap || mem_size > ram_cap - address) {
        PrintError ("ELF file loading failed: "
                    "segment does not fit in interpreter's ram");
        return false;
    }

    const char* data = segment->get_data ();

    if (file_size != 0) {
        if (data == nullptr) {
            PrintError ("ELF file missing segment data");
            return false;
        }

        ram.Store (static_cast<Word>(address), data, file_size);
    }

    if (mem_size > file_size) {
        ram.Set (static_cast<Word>(address + file_size), 
                 0, 
                 mem_size - file_size);
    }

    const auto entry = reader.get_entry ();

    if (!ram.IsAligned (static_cast<Word>(entry)) || entry > ram_cap) {
        PrintError ("Invalid entry point");
        return false;
    }

    *entry_dest = static_cast<Word>(entry);

    return true;
}

//--------------------------------------------------------------------------------

}; // namespace ELF

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————