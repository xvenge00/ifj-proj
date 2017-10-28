#include "gtest/gtest.h"

extern "C"{
#include "../source/scanner.h"
}

namespace {
    class scanner_fixture : public ::testing::Test{
    protected:

        virtual void SetUp(){
            f = fopen("../tests/scanner_test_input/basic_in_2", "r");
            if (f == NULL) {
                printf("NULL\n");
            }
        }

        virtual void TearDown(){
            fclose(f);
        }
    };
}

TEST_F(scanner_fixture, test1){

    //t_token token = get_token();

    EXPECT_TRUE(1==1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}