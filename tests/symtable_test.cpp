#include "gtest/gtest.h"
#include "../source/symtable.h"
//#include "../source/symtable.h"

extern "C" {
#include "../source/symtable.h"
}

namespace {
    class symtable_fixture : public ::testing::Test {
    protected:
        TTable *table;

        virtual void SetUp(){
            table = Tbl_Create();
        }

        virtual void TearDown(){
            free(table);
        }
    };
}

TEST_F(symtable_fixture, initialization){
    EXPECT_NE(table, nullptr);
    EXPECT_EQ(table->size, 8);  //DEFAULT_TABLE_SIZE
    EXPECT_EQ(table->count, 0);
    EXPECT_EQ(table->list_firsts, nullptr); //je prazdna
}

//TEST_F(symtable_fixture, var_create){
//    TType type = E_integer;
//    TValue val; //TODO
//    t_str_buff meno = str_bu
//    TVariable *var = Var_Create(val, type, )
//
//    TElement *elem = El_Create()
//    EXPECT_NE()
//}

//TEST_F(symtable_fixture, insert_elem){
//
//}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}