#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include "config.hpp"
#include <cstdlib>
#include <vector>
#include <stdexcept>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

constexpr std::size_t kRamSize = 1024 * 1024;

//==================================================

class RAM
{
private:
    Byte* data_;
    std::size_t capacity_;

public:
    RAM () : data_(new Byte[kRamSize]), capacity_(kRamSize) {}

    ~RAM ()
    {
        delete [] data_;
    }

    void StoreWord (Word addr, GPRValue data)
    {
        if (capacity_ <= addr) {
            throw std::runtime_error ("Memory: out of bounds");
        }

        *((GPRValue*)(data_ + addr)) = data;
    }

    GPRValue LoadWord (Word addr)
    {
        if (capacity_ <= addr) {
            throw std::runtime_error ("Memory: out of bounds");
        }

        return *((GPRValue*)(data_ + addr));
    }

    bool IsAligned (Word addr)
    {
        return (addr % sizeof (Word)) == 0;
    }
};

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————