#include "gtest/gtest.h"

extern "C" {
#include "../source/memwork.h"
}

namespace {
    class memwork_fixture : public ::testing::Test{
    protected:
    };
}

TEST_F(memwork_fixture, test1){
    //TODO
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}