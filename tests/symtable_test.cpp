#include "gtest/gtest.h"

extern "C" {
#include "../source/symtable.h"
}

/*
 * vyhadzuje kopy warningov
 * nepoznam jednoduchy sposob ako konvretovat c++ string na c string
 * TODO
 */

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

TEST_F(variable_fixture, var_create_int) {
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

TEST_F(variable_fixture, var_create_double) {
    type = E_double;

    //vytvor bezne
    d = 10.0;
    val.d = d;
    EXPECT_NO_FATAL_FAILURE(variable = Var_Create(val, type));
    EXPECT_EQ(variable->var->type, E_double);
    EXPECT_EQ(variable->var->value.d, d);

}

TEST_F(variable_fixture, var_create_string) {
    type = E_string;

    s = "ahoj";
    val.s = s;
    EXPECT_NO_FATAL_FAILURE(variable = Var_Create(val, type));
    EXPECT_EQ(variable->var->value.s, s);
    EXPECT_TRUE(strcmp(s, variable->var->value.s) == 0);
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

TEST_F(function_fixture, func_create_empty) {
    retType = E_integer;
    count = 0;

    EXPECT_NO_FATAL_FAILURE(function = Func_Create(retType, count, NULL));
    EXPECT_EQ(function->func->attr_count, count);
    EXPECT_EQ(function->func->return_type, retType);
    EXPECT_EQ(function->func->attributes, nullptr);

    attr[0] = E_integer;
    EXPECT_NO_FATAL_FAILURE(function = Func_Create(retType, count, attr));
    EXPECT_EQ(function, nullptr);
}

TEST_F(function_fixture, function_create_1_par) {
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

        virtual void SetUp() {
            TValue value;
            value.i = 10;
            data = Var_Create(value, E_integer);
        }
    };
}

TEST_F(symbol_fixture, symbol_create) {
    sym_type = ST_Variable;
    EXPECT_NO_FATAL_FAILURE(symbol = Sym_Create(sym_type, data, name));
    EXPECT_NE(symbol, nullptr);
    EXPECT_EQ(symbol->type, sym_type);
    EXPECT_EQ(symbol->data, data);
    EXPECT_EQ(symbol->name, name);
}

TEST_F(symbol_fixture, symbol_create_null) {
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

        virtual void SetUp() {
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
    EXPECT_NO_FATAL_FAILURE(element = El_Create(NULL));
    EXPECT_EQ(element, nullptr);
}

namespace {
    class symtable_fixture : public ::testing::Test {
    protected:
        TSymbol *symbol;
        TElement *element;
        TTable *table;
        char *name = "meno";

        virtual void SetUp() {
            Symbol_type sym_type;
            TValue value;
            value.i = 10;

            TData *data = Var_Create(value, E_integer);
            symbol = Sym_Create(ST_Variable, data, name);
            element = El_Create(symbol);

        }

        virtual void TearDown() {
            free(table);
        }
    };
}

TEST_F(symtable_fixture, initialization) {
    EXPECT_NO_FATAL_FAILURE(table = Tbl_Create(8));
    EXPECT_NE(table, nullptr);
    EXPECT_EQ(table->size, 8);  //DEFAULT_TABLE_SIZE
    EXPECT_EQ(table->count, 0);
    //zle EXPECT_EQ(table->list_firsts, nullptr); //je prazdna
    //vsetky prvky v table->list_firsts
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
    EXPECT_EQ(table->count, 0); //zatial to funguje opacne
}

#define SYM_COUNT 5

namespace {
    class symtable_insert : public ::testing::Test {
    protected:
        TSymbol *symbol[SYM_COUNT];
        TElement *element[SYM_COUNT];
        char *name[SYM_COUNT] = {"meno1", "meno2", "meno3", "meno4", "meno5"};
        TTable *table;

        virtual void SetUp() {
            Symbol_type sym_type;
            TValue value;
            value.i = 10;

            TData *data = Var_Create(value, E_integer);
            for (int i = 0; i < SYM_COUNT; i++) {
                symbol[i] = Sym_Create(sym_type, data, name[i]);
                element[i] = El_Create(symbol[i]);
            }

        }

        virtual void TearDown() {
            free(table);
        }
    };
}

TEST_F(symtable_insert, insert_more_elements) {
    table = Tbl_Create(4);

    //nacitaj doplna
    for (int i = 0; i < SYM_COUNT-1; i++) {
        EXPECT_NO_FATAL_FAILURE(Tbl_Insert(table, element[i]));
        EXPECT_EQ(table->count, i+1);
    }

    //tabulka sa zvacsila
    EXPECT_EQ(table->size, 8);

    //nacitaj dalsie
    EXPECT_NO_FATAL_FAILURE(Tbl_Insert(table, element[4]));
    EXPECT_EQ(table->count, 5);

    //skontroluj ci su tam vsetky prvky
    for (int i = 0; i < SYM_COUNT; i++) {
        EXPECT_TRUE(Tbl_Search(table, name[i]));
    }
}

TEST_F(symtable_insert, insert_again) {
    table = Tbl_Create(4);
    int success;

    Tbl_Insert(table, element[0]);

    //vloz znova
    EXPECT_NO_FATAL_FAILURE(success = Tbl_Insert(table, element[0]));
    EXPECT_TRUE(success);
}

namespace {
    class symtable_delete : public ::testing::Test {
    protected:
        TSymbol *symbol[SYM_COUNT];
        TElement *element[SYM_COUNT];
        char *name[SYM_COUNT] = {"meno0", "meno1", "meno2", "meno3", "meno4"};
        TTable *table;

        virtual void SetUp() {
            Symbol_type sym_type;
            TValue value;
            value.i = 10;

            TData *data = Var_Create(value, E_integer);
            for (int i = 0; i < SYM_COUNT; i++) {
                symbol[i] = Sym_Create(sym_type, data, name[i]);
                element[i] = El_Create(symbol[i]);
            }

        }

        virtual void TearDown() {
            free(table);
        }
    };
}

TEST_F(symtable_delete, el_delete) {
    table = Tbl_Create(4);

    //nacitaj
    for (int i = 0; i < SYM_COUNT-1; i++) {
        Tbl_Insert(table, element[i]);
    }

    EXPECT_TRUE(Tbl_Search(table, "meno1"));
    EXPECT_NO_FATAL_FAILURE(El_Delete(table, "meno1"));
    EXPECT_FALSE(Tbl_Search(table, "meno1"));
}

TEST_F(symtable_delete, el_delete_empty) {
    table = Tbl_Create(4);

    EXPECT_FALSE(Tbl_Search(table, "meno1"));
    EXPECT_NO_FATAL_FAILURE(El_Delete(table, "meno1"));
    EXPECT_FALSE(Tbl_Search(table, "meno1"));
}

TEST_F(symtable_delete, el_delete_other) {
    table = Tbl_Create(4);

    for (int i = 0; i < SYM_COUNT-1; i++) {
        Tbl_Insert(table, element[i]);
    }

    EXPECT_FALSE(Tbl_Search(table, "meno8"));
    EXPECT_NO_FATAL_FAILURE(El_Delete(table, "meno8"));
    EXPECT_FALSE(Tbl_Search(table, "meno8"));
}

TEST_F(symtable_delete, el_delete_null) {
    table = Tbl_Create(4);

    EXPECT_NO_FATAL_FAILURE(El_Delete(table, NULL));
}

namespace {
    class symtable_search : public ::testing::Test {
    protected:
        char *name[SYM_COUNT] = {"meno0", "meno1", "meno2", "meno3", "meno4"};
        TTable *table;
        TElement *element[SYM_COUNT];
        int success;

        virtual void SetUp() {
            Symbol_type sym_type;
            TValue value;
            value.i = 10;
            TSymbol *symbol[SYM_COUNT];


            table = Tbl_Create(4);

            TData *data = Var_Create(value, E_integer);
            for (int i = 0; i < SYM_COUNT; i++) {
                symbol[i] = Sym_Create(sym_type, data, name[i]);
                element[i] = El_Create(symbol[i]);
            }

        }

        virtual void TearDown() {
            free(table);
        }
    };
}

TEST_F(symtable_search, search_empty) {
    table = Tbl_Create(4);

    EXPECT_NO_FATAL_FAILURE(success = Tbl_Search(table, "ahoj"));
    EXPECT_FALSE(success);
}


TEST_F(symtable_search, search_null) {
    EXPECT_NO_FATAL_FAILURE(success = Tbl_Search(NULL, "ahoj"));
    EXPECT_FALSE(success);

    EXPECT_NO_FATAL_FAILURE(success =  Tbl_Search(table, NULL));
    EXPECT_FALSE(success);

    EXPECT_NO_FATAL_FAILURE(success =  Tbl_Search(NULL, NULL));
    EXPECT_FALSE(success);
}

TEST_F(symtable_search, search_existing) {
    for (int i = 0; i < SYM_COUNT; i++) {
        Tbl_Insert(table, element[i]);
    }

    for (int i = 0; i < SYM_COUNT; i++) {
        EXPECT_TRUE(Tbl_Search(table, name[i]));
    }

    EXPECT_FALSE(Tbl_Search(table, "ahoj"));
}

//TODO Tbl_copy()
//TODO El_free test


//TODO tbl_delete
//TODO el_delete


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}