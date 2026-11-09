#include "bt_cache.hpp"
#include <cassert>
#include <ir.h>

//--------------------------------------------------------------------------------

namespace toy_isa_interpreter
{

//--------------------------------------------------------------------------------

BTCache::~BTCache ()
{
    for (auto it = map_.begin (), it_end = map_.end (); it != it_end; ++it) {
        BlockTranslated& bt = it->second;
        assert (bt.entry_);
        ir_mem_unmap (reinterpret_cast<void*>(bt.entry_), bt.size_);
    }
}

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
