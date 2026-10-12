#include <gtest/gtest.h>
#include "cpu.hpp"
#include "decode.hpp"
#include "test_decode.hpp"
#include "test_execute.hpp"

//--------------------------------------------------------------------------------

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest (&argc, argv);

    return RUN_ALL_TESTS ();
}

//--------------------------------------------------------------------------------