#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include "config.hpp"
#include "decode.hpp"
#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <cstring>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

constexpr std::size_t kRamSize = 1024 * 1024;

//==================================================

// FIXME: вместо загрузки по Word, делать memcpy
class RAM
{
private:
    Byte* data_;
    std::size_t capacity_;

    void CheckAlignment (Word address) const
    {
        if (!IsAligned (address)) {
            throw std::runtime_error ("Misaligned address");
        }
    }

    void CheckBounds (Word address, std::size_t data_size) const
    {
        if (capacity_ < address || capacity_ - address < data_size) {
            throw std::runtime_error ("Memory: out of bounds");
        }
    }

public:
    RAM () : data_(new Byte[kRamSize]), capacity_(kRamSize) {}

    ~RAM ()
    {
        delete [] data_;
    }

    std::size_t GetCapacity () const
    {
        return capacity_;
    }

    void Store (Word address, const void* data, std::size_t size)
    {
        assert (data != nullptr);
        CheckBounds (address, size);

        std::memcpy (data_ + address, data, size);
    }

    void Load (Word address, void* dest, std::size_t size) const
    {
        assert (dest != nullptr);
        CheckBounds (address, size);

        std::memcpy (dest, data_ + address, size);
    }

    void Set (Word address, Byte what, std::size_t times)
    {
        CheckBounds (address, sizeof (what) * times);

        std::memset (data_ + address, what, times);
    }

    void StoreWord (Word addr, GPRValue data)
    {
        Store (addr, &data, sizeof (GPRValue));
    }

    GPRValue LoadWord (Word addr) const
    {
        GPRValue value {};

        Load (addr, &value, sizeof (GPRValue));

        return value;
    }

    bool IsAligned (Word addr) const
    {
        return (addr % sizeof (Word)) == 0;
    }
};

//————————————————————————————————————————————————————————————————————————————————

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————