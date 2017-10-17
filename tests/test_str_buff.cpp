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
    EXPECT_NE(buffer, nullptr);     //je alokovany
    EXPECT_EQ(buffer->top,0);       //top je nula
    EXPECT_TRUE(buffer->max > 0);
}


TEST_F(str_buff_fixture, null_buffer_empty){
    //vyprazdni prazdny
    EXPECT_NO_FATAL_FAILURE(null_buffer(buffer));
    EXPECT_DEATH(null_buffer(NULL), ".*");
}

TEST_F(str_buff_fixture, append_buff_empty){
    //ci pri NULL padne
    EXPECT_DEATH(append_buff(NULL, 'a'), ".*");

    //prazny
    EXPECT_NO_FATAL_FAILURE(append_buff(buffer, 'a'));
    EXPECT_EQ(buffer->ret[0],'a');
    EXPECT_EQ(buffer->ret[1], (char) 0);    //retazec je ukonceny 0
    EXPECT_EQ(buffer->top, 1);              //skontroluj kde je top
    EXPECT_TRUE(buffer->top <= buffer->max);
}

TEST_F(str_buff_fixture, append_buff_viac_krat){
    EXPECT_NO_FATAL_FAILURE(append_buff(buffer, 'a'));  // vloz prvy

    EXPECT_NO_FATAL_FAILURE(append_buff(buffer, 'b'));  //append druhy krat
    EXPECT_EQ(buffer->ret[1],'b');
    EXPECT_EQ(buffer->ret[2], (char) 0);
    EXPECT_EQ(buffer->top, 2);
}

//TEST_F(str_buff_fixture, append_after_new){
//    //TODO
//}
//
//TEST_F(str_buff_fixture, new_after_append){
//    //TODO
//}

TEST_F(str_buff_fixture, append_full){
    buffer->top = 2;
    buffer->max = 2;
    //TODO test ked sa nemesti do str_buff
}

TEST_F(str_buff_fixture, novy_string){
    //prirad string
    EXPECT_NO_FATAL_FAILURE(buffer->ret = "test");
    EXPECT_EQ(buffer->ret[5], (char) 0);

    //skontroluj kolko je top
    EXPECT_EQ(buffer->top, 5);

    //skontroluj ci je max>=top
    EXPECT_TRUE(buffer->max >= buffer->top);
}

TEST_F(str_buff_fixture, null_buffer_1_elem){
    append_buff(buffer, 'a');

    unsigned max = buffer->max;

    EXPECT_NO_FATAL_FAILURE(null_buffer(buffer));
    EXPECT_EQ(buffer->top, 0);
    EXPECT_EQ(buffer->max, max);    //TODO ci sa zmenilo max na default alebo zostalo
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}