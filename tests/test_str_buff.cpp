#include "gtest/gtest.h"

extern "C" {
#include "../source/str_buff.h"
}

namespace{
    class str_buff_fixture : public ::testing::Test {
    protected:
        t_str_buff *buffer;

        virtual void SetUp(){
            buffer = init_buff();
        }

        virtual void TearDown(){
            free(buffer);
        }
    };

};

TEST_F(str_buff_fixture, init){
    EXPECT_NE(buffer, nullptr);    //je alokovany
    EXPECT_EQ(buffer->top,0);   //top je nula
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}