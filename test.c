#include "greatest.h"
#include "libmguard.h"

/*
 * mg_hash_returns_a_hash
 *
 * When given data the mg_hash function should return a hash.
 */
TEST
mg_hash_returns_a_hash(void)
{
  int data = 31337;
  unsigned int hash = mg_hash((char*)&data, sizeof(data));

  ASSERT_NEQ(0, hash);

  PASS();
}

/*
 * mg_hash_same_data_same_hash
 *
 * The same has should result from the same data. For example if the data passed
 * in is "31337" then the hash function should always return the same value.
 *
 */
TEST
mg_hash_same_data_same_hash(void)
{
  int data0 = 31337;
  int data1 = 31337;
  unsigned int hash0 = mg_hash((char *)&data0, sizeof(data0));
  unsigned int hash1 = mg_hash((char *)&data1, sizeof(data1));

  ASSERT_EQ(hash0, hash1);

  PASS();
}

/*
 * mg_hash_changes_with_data
 *
 * Ensure a different hash result is returned with different data. There would
 * obviously be a problem if the hash function is returning the same result with
 * different data supplied.
 *
 */
TEST
mg_hash_changes_with_data(void)
{
  int data = 31337;
  unsigned int hash = mg_hash((void *)&data, sizeof(data));

  data = 43922;
  unsigned int new_hash = mg_hash((void *)&data, sizeof(data));

  ASSERT_NEQ(hash, new_hash);

  PASS();
}

/*
 * mg_hash_changes_with_any_data
 *
 * Ensure the hash is always different, even with different data types, sizes
 * etc.
 *
 */
TEST
mg_hash_changes_with_any_data(void)
{
  int data0 = 31337;
  char *data1 = "Snafu and fubar are present.";

  unsigned int hash0 = mg_hash((char *)&data0, sizeof(data0));
  unsigned int hash1 = mg_hash((char *)&data1, sizeof(data1));

  ASSERT_NEQ(hash0, hash1);

  PASS();
}

/*
 * mg_init_context_sets_up_context(void)
 *
 * When a memory guard context is created memory should be allocated for the
 * array, the used value should be set to zero (0), and the size should be set
 * to one (1).
 *
 */
TEST
mg_init_context_sets_up_context(void)
{
  MGContext context;
  int result = mg_init_context(&context);

  ASSERT_EQ(1, result);
  ASSERT_EQ(0, context.used);
  ASSERT_EQ(1, context.size);

  PASS();
}

/*
 * mg_init_context_can_create_more_than_one(void)
 *
 * Creating more than one context should produce two different contexts within
 * memory. The contexts should not be shared or the same one.
 *
 */
TEST
mg_init_context_can_create_more_than_one(void)
{
  MGContext context0;
  MGContext context1;

  int result0 = mg_init_context(&context0);
  int result1 = mg_init_context(&context1);

  ASSERT_EQ(1, result0);
  ASSERT_EQ(1, result1);

  ASSERT_NEQ(&result0, &result1);

  PASS();
}

/*
 * mg_protect_adds_variable_to_context
 *
 * Adds a variable to the context as protected.
 *
 */
TEST
mg_protect_adds_variable_to_context(void)
{
  int data = 31337;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data, sizeof(data)));

  PASS();
}

/*
 * mg_protect_does_not_add_existing_variable
 *
 * When a variable is added to the context, it should only be allowed to be
 * added once. When the same data is added to the context, nothing should
 * be added or even really happen.
 *
 */
TEST
mg_protect_does_not_add_existing_variable(void)
{
  int result;
  int data = 31337;
  MGContext context;

  result = mg_init_context(&context);

  ASSERT_EQ(1, mg_protect(&context, &data, sizeof(data)));
  ASSERT_EQ(0, mg_protect(&context, &data, sizeof(data)));

  PASS();
}

/*
 * mg_protect_does_not_increase_size
 *
 * When the very first variable is added to a context the size of the array
 * should be correct. The size of the array should be one (1). Essentially we
 * are ensuring that the default count of one (1) preserved during the first
 * variable being added.
 *
 */
TEST
mg_protect_does_not_increase_size(void)
{
  int result;
  int data = 31337;
  MGContext context;

  result = mg_init_context(&context);
  result = mg_protect(&context, &data, sizeof(data));

  ASSERT_EQ(1, context.size);

  PASS();
}

/*
 * mg_protect_increases_count
 *
 * When additional variables are added to a context after the first one then
 * the count should start increasing.
 *
 */
TEST
mg_protect_increases_count(void)
{
  int result;
  int data0 = 31337;
  int data1 = 34922;
  int data2 = 11350;
  int data3 = 11550;
  MGContext context;

  result = mg_init_context(&context);

  ASSERT_EQ(1, mg_protect(&context, &data0, sizeof(data0)));
  ASSERT_EQ(2, mg_protect(&context, &data1, sizeof(data1)));
  ASSERT_EQ(3, mg_protect(&context, &data2, sizeof(data2)));
  ASSERT_EQ(4, mg_protect(&context, &data3, sizeof(data3)));

  PASS();
}

/*
 * mg_protected_p_returns_position_if_protected
 *
 * When a variable is being passed to the function and it is protected then the
 * array position is returned. So, any number greater than zero (0) means the
 * variable is protected.
 *
 */
TEST
mg_protected_p_returns_position_if_protected(void)
{
  int data, snafu = 31337;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data, sizeof(data)));
  ASSERT_EQ(0, mg_protected_p(&context, &data));
  
  PASS();
}

/*
 * mg_protected_p_returns_negative_if_not_protected
 *
 * When a variable is being passed and it is not protected then the result will
 * be a negative-one (-1).
 *
 */
TEST
mg_protected_p_returns_negative_if_not_protected(void)
{
  int data0 = 31337;
  int data1 = 34922;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data0, sizeof(data0)));
  ASSERT_EQ(-1, mg_protected_p(&context, &data1));

  PASS();
}

/*
 * mg_update_creates_new_hash
 *
 * When updating a variable's value a new hash is created.
 *
 */
TEST
mg_update_creates_new_hash(void)
{
  int data = 31337;
  unsigned int old_hash, new_hash;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data, sizeof(data)));

  data = 34922;
  new_hash = mg_hash((char *)&data, sizeof(data));
  old_hash = context.array[0].hash;

  ASSERT_EQ(sizeof(data), mg_update(&context, &data));
  ASSERT_EQ(new_hash, context.array[0].hash);
  ASSERT_NEQ(old_hash, context.array[0].hash);
  
  PASS();
}

/*
 * mg_update_returns_size
 *
 * When an update is successful the size of the data is returned.
 *
 */
TEST
mg_update_returns_size(void)
{
  int data = 31337;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data, sizeof(data)));

  data = 34922;

  ASSERT_EQ(sizeof(data), mg_update(&context, &data));
  
  PASS();
}

/*
 * mg_update_returns_nothing_if_not_protected
 *
 * When an updated is attempted and the variable passed is not protected then
 * zero (0) is returned.
 *
 */
TEST
mg_update_returns_nothing_if_not_protected(void)
{
  int data0 = 31337;
  int data1 = 34922;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data0, sizeof(data0)));

  ASSERT_EQ(0, mg_update(&context, &data1));
  
  PASS();
}

/*
 * mg_compromised_p_returns_zero_if_not_compromised
 *
 * When checking if data has been compromised return zero (0) if the data has
 * not been compromised and is secure.
 *
 */
TEST
mg_compromised_p_returns_zero_if_not_compromised(void)
{
  int data = 31337;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data, sizeof(data)));

  ASSERT_EQ(0, mg_compromised_p(&context, &data));
  
  PASS();
}

/*
 * mg_compromised_p_returns_one_if_compromised
 *
 * When checking if data has been compromised return one (1) if the data has
 * in fact been compromised.
 *
 */
TEST
mg_compromised_p_returns_one_if_compromised(void)
{
  int data = 31337;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data, sizeof(data)));

  data = 34922;
  
  ASSERT_EQ(1, mg_compromised_p(&context, &data));
  
  PASS();
}

/*
 * mg_compromised_p_returns_one_if_not_protected
 *
 * This function can be asked if a variable not even being protected has been
 * compromised. When this is asked it should be treated as a compromised
 * variable. Therefore, one (1) should be returned.
 *
 */
TEST
mg_compromised_p_returns_one_if_not_protected(void)
{
  int data0 = 31337;
  int data1 = 34922;
  MGContext context;

  ASSERT_EQ(1, mg_init_context(&context));
  ASSERT_EQ(1, mg_protect(&context, &data0, sizeof(data0)));

  data0 = 34922;
  
  ASSERT_EQ(1, mg_compromised_p(&context, &data1));
  
  PASS();
}

/*
 * mg_stress_test
 *
 * This is a light, fast moving stress test using two different MGContexts.
 * The test is to ensure library functions can be called many times quickly
 * without any memory or other errors.
 *
 */
struct bank_account {
  char account_number[16];
  int balance;
};

struct bank_transfer {
  char from_account_number[16];
  char to_account_number[16];
  int amount;
};

TEST
mg_stress_test(void)
{
  struct bank_account accounts[128];
  struct bank_transfer transfers[1024];

  for (int i = 0; i < 128; i++) {
    sprintf(accounts[i].account_number, "%015i", i);
    accounts[i].balance = (rand() % 100000);
  }

  for (int i = 0; i < 1024; i++) {
    int from_acct = (rand() % 128);
    int to_acct = (rand() % 128);

    if (from_acct == to_acct)
      to_acct++;

    sprintf(transfers[i].from_account_number, "%015i", from_acct);
    sprintf(transfers[i].to_account_number, "%015i", to_acct);
    transfers[i].amount = (rand() % 100000);
  }

  MGContext mgc_accounts;
  MGContext mgc_transfers;

  if (!mg_init_context(&mgc_accounts))
    FAIL();

  if (!mg_init_context(&mgc_transfers))
    FAIL();

  for (int i = 0; i < 128; i++)
    mg_protect(&mgc_accounts, &accounts[i], sizeof(accounts[i]));

  for (int i = 0; i < 1024; i++)
    mg_protect(&mgc_transfers, &transfers[i], sizeof(transfers[i]));

  for (int i = 0; i < 1024; i++) {
    int from_acct = atoi(transfers[i].from_account_number);
    int to_acct = atoi(transfers[i].to_account_number);

    if (i == 2 || i == 4 || i == 8 || i == 16 || i == 32 || i == 64) {
      accounts[i].balance = accounts[i].balance + 100;

      ASSERT(mg_compromised_p(&mgc_accounts, &accounts[i]));
      ASSERT(mg_update(&mgc_accounts, &accounts[i]));
    }
    
    if (!mg_compromised_p(&mgc_accounts, &accounts[from_acct]) &&
        !mg_compromised_p(&mgc_accounts, &accounts[to_acct]) &&
        !mg_compromised_p(&mgc_transfers, &transfers[i])) {

      accounts[from_acct].balance = accounts[from_acct].balance - transfers[i].amount;
      accounts[to_acct].balance = accounts[to_acct].balance + transfers[i].amount;

      ASSERT(mg_update(&mgc_accounts, &accounts[from_acct]));
      ASSERT(mg_update(&mgc_accounts, &accounts[to_acct]));
    }
  }
  
  PASS();
}

SUITE(hash)
{
  RUN_TEST(mg_hash_returns_a_hash);
  RUN_TEST(mg_hash_same_data_same_hash);
  RUN_TEST(mg_hash_changes_with_data);
  RUN_TEST(mg_hash_changes_with_any_data);
}

SUITE(init_context)
{
  RUN_TEST(mg_init_context_sets_up_context);
  RUN_TEST(mg_init_context_can_create_more_than_one);
}

SUITE(protect)
{
  RUN_TEST(mg_protect_adds_variable_to_context);
  RUN_TEST(mg_protect_does_not_add_existing_variable);
  RUN_TEST(mg_protect_increases_count);
}

SUITE(protected_p)
{
  RUN_TEST(mg_protected_p_returns_position_if_protected);
  RUN_TEST(mg_protected_p_returns_negative_if_not_protected);
}

SUITE(update)
{
  RUN_TEST(mg_update_creates_new_hash);
  RUN_TEST(mg_update_returns_size);
  RUN_TEST(mg_update_returns_nothing_if_not_protected);
}

SUITE(compromised_p)
{
  RUN_TEST(mg_compromised_p_returns_zero_if_not_compromised);
  RUN_TEST(mg_compromised_p_returns_one_if_compromised);
  RUN_TEST(mg_compromised_p_returns_one_if_not_protected);
}

SUITE(stress)
{
  RUN_TEST(mg_stress_test);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char **argv)
{
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(hash);
  RUN_SUITE(init_context);
  RUN_SUITE(protect);
  RUN_SUITE(update);
  RUN_SUITE(compromised_p);
  RUN_SUITE(stress);

  GREATEST_MAIN_END();

  return 0;
}
  
