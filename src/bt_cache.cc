#include "bt_cache.hpp"
#include <cassert>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

bool BTCache::Lookup (PCValue pc, BlockTranslated& dest)
{
    auto it = map_.find (pc);

    if (it == map_.end ()) {
        // no translated block by pc
        return false;
    }

    dest = it->second;

    return true;
}

//--------------------------------------------------------------------------------

void BTCache::Add (PCValue pc, BlockTranslated& block)
{
    assert (map_.find (pc) == map_.end ());

    map_[pc] = block;
}

//--------------------------------------------------------------------------------

}; // namespace toy_isa_interpreter

//--------------------------------------------------------------------------------
