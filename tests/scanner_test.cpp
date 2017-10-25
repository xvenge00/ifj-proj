#include "gtest/gtest.h"

extern "C"{
#include "../source/scanner.h"
}

namespace {
    class scanner_fixture : public ::testing::Test{
    protected:
    };
}

TEST_F(scanner_fixture, test1){
    //TODO
}

//TODO NEVIEM TESTOVAT VSTUP ZO stdin !!!!!!

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}