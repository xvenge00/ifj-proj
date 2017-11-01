#include "gtest/gtest.h"


extern "C"{
#include "../source/scanner.h"
}


namespace {

    class scanner_fixture_basic_1 : public ::testing::Test{
    protected:

        virtual void SetUp(){
            f = fopen("../tests/scanner_test_input/basic_in", "r");
            if (f == NULL) {
                printf("NULL\n");
            }
        }

        virtual void TearDown(){
            fclose(f);
        }
    };

    class scanner_fixture_basic_2 : public ::testing::Test{
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

    class scanner_fixture_comment : public ::testing::Test{
    protected:

        virtual void SetUp(){
            f = fopen("../tests/scanner_test_input/basic_in_comment", "r");
            if (f == NULL) {
                printf("NULL\n");
            }
        }

        virtual void TearDown(){
            fclose(f);
        }
    };

}

TEST_F(scanner_fixture_basic_1, basic_test1){
    const int size = 15;

    t_token *tmp;
    int i = 0;


    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_scope);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_end);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_scope);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);


    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EMPTY);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EMPTY);
    EXPECT_TRUE(tmp->data.s == NULL);




}


TEST_F(scanner_fixture_basic_2, basic_test2){
    t_token *tmp;
    int i = 0;


    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_scope);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);


    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_dim);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, ID);
    EXPECT_STREQ(tmp->data.s, "a");

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_as);


    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_string);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EQ);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, STR);
    EXPECT_STREQ(tmp->data.s, "");

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_end);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_scope);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EMPTY);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EMPTY);
    EXPECT_TRUE(tmp->data.s == NULL);




}

TEST_F(scanner_fixture_comment, comment_test){
    t_token *tmp = NULL;

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_string);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_scope);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_dim);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, ID);
    EXPECT_STREQ(tmp->data.s, "a");

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_as);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_string);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EQ);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, STR);
    EXPECT_STREQ(tmp->data.s, "");

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EOL);
    EXPECT_TRUE(tmp->data.s == NULL);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_end);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, KEY_WORD);
    EXPECT_EQ(tmp->data.i, k_scope);

    tmp = get_token();
    EXPECT_EQ(tmp->token_type, EMPTY);
    EXPECT_TRUE(tmp->data.s == NULL);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}