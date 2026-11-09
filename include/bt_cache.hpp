#pragma once

//————————————————————————————————————————————————————————————————————————————————

#include "decode.hpp"
#include <unordered_map>

//————————————————————————————————————————————————————————————————————————————————

namespace toy_isa_interpreter
{

//————————————————————————————————————————————————————————————————————————————————

using TranslatedFunc = void (*) ();

//————————————————————————————————————————————————————————————————————————————————

struct BlockTranslated
{
    TranslatedFunc entry_;
    std::size_t    size_;
};

//==================================================

class BTCache
{
private:
    using BTCacheMap = std::unordered_map<PCValue, BlockTranslated>;

    BTCacheMap map_;

public:
    std::size_t GetSize () const
    {
        return map_.size ();
    }

    bool Lookup (PCValue pc, BlockTranslated& dest);

    void Add (PCValue pc, BlockTranslated& block);
};

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//————————————————————————————————————————————————————————————————————————————————
