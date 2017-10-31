#include "gtest/gtest.h"

extern "C" {
#include "../source/symtable.h"
}

namespace {
    class variable_fixture : public ::testing::Test {
    protected:
        int i;
        double d;
        char *s;
        TType type;
        TValue val;
        TData *variable;

    };
}

TEST_F(variable_fixture, var_create_int){
    i = 10;
    type = E_integer;
    val.i = i;

    //vtvor bezne
    EXPECT_NO_FATAL_FAILURE(variable = Var_Create(val, type));
    EXPECT_EQ(variable->var->type, E_integer);
    EXPECT_EQ(variable->var->value.i, i);

    //vytvor hranicne
    i = INT32_MAX;
    val.i = i;
    EXPECT_NO_FATAL_FAILURE(variable = Var_Create(val, type));
    EXPECT_EQ(variable->var->type, E_integer);
    EXPECT_EQ(variable->var->value.i, i);

    i = INT32_MIN;
    val.i = i;
    EXPECT_NO_FATAL_FAILURE(variable = Var_Create(val, type));
    EXPECT_EQ(variable->var->type, E_integer);
    EXPECT_EQ(variable->var->value.i, i);

}

TEST_F(variable_fixture, var_create_double){
    type = E_double;

    //vytvor bezne
    d = 10.0;
    val.d = d;
    EXPECT_NO_FATAL_FAILURE(variable = Var_Create(val, type));
    EXPECT_EQ(variable->var->type, E_double);
    EXPECT_EQ(variable->var->value.d, d);

}

TEST_F(variable_fixture, var_create_string){
    type = E_string;

    s = "ahoj";
    val.s = s;
    EXPECT_NO_FATAL_FAILURE(variable = Var_Create(val, type));
    EXPECT_EQ(variable->var->value.s, s);
    EXPECT_TRUE(strcmp(s, variable->var->value.s)==0);
    EXPECT_EQ(variable->var->type, E_string);
}

namespace {
    class function_fixture : public ::testing::Test {
    protected:
        TData *function;
        TType retType;
        unsigned int count;
        TType attr[10];
    };
}

TEST_F(function_fixture, func_create_empty){
    retType = E_integer;
    count = 0;

    EXPECT_NO_FATAL_FAILURE(function = Func_Create(retType,count, NULL));
    EXPECT_EQ(function->func->attr_count, count);
    EXPECT_EQ(function->func->return_type, retType);
    EXPECT_EQ(function->func->attributes, nullptr);

    attr[0] = E_integer;
    EXPECT_NO_FATAL_FAILURE(function = Func_Create(retType, count, attr));
    EXPECT_EQ(function->func->attr_count, count);
    EXPECT_EQ(function->func->return_type, retType);
    EXPECT_EQ(function->func->attributes, nullptr );
}

TEST_F(function_fixture, function_create_1_par){
    retType = E_integer;
    count = 1;
    attr[0] = E_integer;

    EXPECT_NO_FATAL_FAILURE(function = Func_Create(retType, count, attr));
    EXPECT_EQ(function->func->attr_count, count);
    EXPECT_EQ(function->func->return_type, retType);
    EXPECT_EQ(function->func->attributes, attr);

    function = Func_Create(retType, count, NULL);
    EXPECT_EQ(function, nullptr);
}


namespace {
    class symbol_fixture : public ::testing::Test {
    protected:
        TSymbol *symbol;
        Symbol_type sym_type;
        TData *data;
        char *name = "meno";

        virtual void SetUp(){
            TValue value;
            value.i = 10;
            data = Var_Create(value, E_integer);
        }
    };
}

TEST_F(symbol_fixture, symbol_create){
    sym_type = ST_Variable;
    EXPECT_NO_FATAL_FAILURE(symbol = Sym_Create(sym_type, data, name));
    EXPECT_NE(symbol, nullptr);
    EXPECT_EQ(symbol->type, sym_type);
    EXPECT_EQ(symbol->data, data);
    EXPECT_EQ(symbol->name, name);
}

TEST_F(symbol_fixture, symbol_create_null){
    sym_type = ST_Variable;

    //neplatny ukazatel na data
    EXPECT_NO_FATAL_FAILURE(symbol = Sym_Create(sym_type, NULL, name));
    EXPECT_EQ(symbol, nullptr);

    //neplatny ukazatel na name
    EXPECT_NO_FATAL_FAILURE(symbol = Sym_Create(sym_type, data, NULL));
    EXPECT_EQ(symbol, nullptr);
}

namespace {
    class element_fixture : public ::testing::Test {
    protected:
        TSymbol *symbol;
        TElement *element;

        virtual void SetUp(){
            Symbol_type sym_type;
            TValue value;
            value.i = 10;

            TData *data = Var_Create(value, E_integer);
            symbol = Sym_Create(ST_Variable, data, "meno");
        }
    };
}

TEST_F(element_fixture, element_create) {
    EXPECT_NO_FATAL_FAILURE(element = El_Create(symbol));
    EXPECT_NE(element, nullptr);        //platny ukazatel
    EXPECT_EQ(element->data, symbol);
    EXPECT_EQ(element->key, symbol->name);
    EXPECT_EQ(element->next, nullptr);
}

TEST_F(element_fixture, element_create_null) {
    //ked mu dam NULL aby vratil NULL
    //EXPECT_NO_FATAL_FAILURE(element = El_Create(NULL));
    EXPECT_EQ(element, nullptr);
}

//TODO El_free test

namespace {
    class symtable_fixture : public ::testing::Test {
    protected:
        TSymbol *symbol;
        TElement *element;
        TTable *table;
        char *name = "meno";

        virtual void SetUp(){
            Symbol_type sym_type;
            TValue value;
            value.i = 10;

            TData *data = Var_Create(value, E_integer);
            symbol = Sym_Create(ST_Variable, data, name);
            element = El_Create(symbol);

        }

        virtual void TearDown(){
            free(table);
        }
    };
}

TEST_F(symtable_fixture, initialization){
    EXPECT_NO_FATAL_FAILURE(table = Tbl_Create(8));
    EXPECT_NE(table, nullptr);
    EXPECT_EQ(table->size, 8);  //DEFAULT_TABLE_SIZE
    EXPECT_EQ(table->count, 0);
    EXPECT_EQ(table->list_firsts, nullptr); //je prazdna
}

TEST_F(symtable_fixture, insert_element) {
    table = Tbl_Create(8);
    int success;

    EXPECT_NO_FATAL_FAILURE(success = Tbl_Insert(table, element));
    EXPECT_TRUE(success);
    EXPECT_EQ(table->count, 1);
    EXPECT_TRUE(Tbl_Search(table, name));
    EXPECT_EQ(table->size, 8);
}

TEST_F(symtable_fixture, insert_element_null) {
    table = Tbl_Create(8);
    int success;

    //nemalo by sa podarit vlozit neplatny ukaz
    EXPECT_NO_FATAL_FAILURE(success = Tbl_Insert(table, NULL));
    EXPECT_FALSE(success);
    EXPECT_EQ(table->count, 0);
}

TEST_F(symtable_fixture, insert_more_elements) {
    table = Tbl_Create(8);
    int success;

}

/* TODO
 * sprav 8 elementov a vloz
 * sprav 9 elementov a vloz
 * skus si sa podari vlozit uz vlozeny prvok
 * skus si sa rozsiruje tabulka
 */

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}