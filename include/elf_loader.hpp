#include "elfio/elf_types.hpp"
#include <elfio/elfio.hpp>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

namespace ELF
{

//--------------------------------------------------------------------------------

constexpr unsigned char   kClass       = ELFIO::ELFCLASS32;
constexpr unsigned char   kEncoding    = ELFIO::ELFDATA2LSB;
constexpr ELFIO::Elf_Half kMachineCode = 130;
constexpr unsigned char   kOsAbi       = 188;
constexpr unsigned char   kAbiVersion  = 1;

//--------------------------------------------------------------------------------

bool LoadELfFile (const std::string& file_path);

//--------------------------------------------------------------------------------

}; // namespace ELF

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------