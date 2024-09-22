#include <stdio.h>
#include "libmguard.h"

int test_count = 0;
int test_pass = 0;

#define TEST(name) void name(void)

#define RUN_TEST(name)                          \
  do {                                          \
    printf("* TEST %s\n", #name);               \
    name();                                     \
  } while (0)

#define doc(documentation)                      \
  do {                                          \
    printf("       %s\n", documentation);       \
  } while (0)

#define assert(condition, description)          \
  do {                                          \
    test_count++;                               \
    if (condition) {                            \
      test_pass++;                              \
      printf("  PASS %s\n", description);       \
    }                                           \
    else {                                      \
      printf("  FAIL %s\n", description);       \
    }                                           \
  } while (0)

#define PRINT_STATS()                                             \
  do {                                                            \
    printf("* DONE Total Tests: %d\n", test_count);               \
    printf("            Passed: %d\n", test_pass);                \
    printf("            Failed: %d\n", (test_count - test_pass)); \
  } while(0)

TEST(mg_hash_returns_a_hash)
{
  doc("When given data the mg_hash function should return a hash.");

  int data = 34922;
  unsigned int hash = mg_hash((char *)&data, sizeof(data));

  assert(hash != 0, "Ensure the hash does not equal zero.");
}

TEST(mg_hash_same_data_same_hash)
{
  doc("The same hash should result form the same data. If the data passed is");
  doc("the int 34922 then the hash function should always return the same");
  doc("value.");

  int data0 = 34922;
  int data1 = 34922;
  unsigned int hash0 = mg_hash((char *)&data0, sizeof(data0));
  unsigned int hash1 = mg_hash((char *)&data1, sizeof(data1));

  assert(hash0 == hash1, "Both hash values should be equal.");
}

TEST(mg_hash_changes_with_data)
{
  doc("When the hash function should always return a different result when");
  doc("different data is asked to be hashed. Ensure the hash function is not");
  doc("returning the same hash for different data.");

  int data = 31337;
  unsigned int hash = mg_hash((void *)&data, sizeof(data));

  data = 34922;
  unsigned int new_hash = mg_hash((void *)&data, sizeof(data));

  assert(hash != 0, "Ensure hash holds a hash value and is not empty.");
  assert(new_hash != 0, "Ensure new_hash holds a hash value and is not empty.");
  assert(hash != new_hash, "Ensure hash and new_hash do not hold the same hash value.");
}

TEST(mg_hash_changes_with_any_data)
{
  doc("Ensure the hash is always different, even with different data types");
  doc("and data sizes, etc.");

  int data0 = 34922;
  char *data1 = "Snafu and fubar are present.";

  unsigned int hash0 = mg_hash((char *)&data0, sizeof(data0));
  unsigned int hash1 = mg_hash((char *)&data1, sizeof(data1));

  assert(hash0 != 0, "Ensure hash holds a value and is not empty.");
  assert(hash1 != 0, "Ensure hash holds a value and is not empty.");
  assert(hash0 != hash1, "Ensure the two hashes are not equal to each other.");
}

TEST(mg_init_context_sets_up_context)
{
  doc("When a memory guard context is created memory should be allocated for the");
  doc("array, the used value should be set to zero (0), and the size should be");
  doc("set to one (1).");

  MGContext context;
  int result = mg_init_context(&context);

  assert(result == 1, "The context should initialize correctly.");
  assert(context.used == 0, "Used should be zero (0).");
  assert(context.size == 1, "Size should be one (1).");
}

TEST(mg_init_context_can_create_more_than_one)
{
  doc("Creating more than one context should produce two different contexts");
  doc("within memory. The contexts should not be shared.");

  MGContext context0;
  MGContext context1;

  int result0 = mg_init_context(&context0);
  int result1 = mg_init_context(&context1);

  assert(result0 == 1, "Context zero should init correctly.");
  assert(result1 == 1, "Context one should init correctly.");
  assert(&result0 != &result1, "Context zero should not be context one.");
}

TEST(mg_protect_adds_variable_to_context)
{
  doc("Adds a variable to the context as protected.");

  int data = 34922;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data, sizeof(data)) == 1, "The variable 'data' should be protected.");
}

TEST(mg_protect_does_not_add_existing_variable)
{
  doc("When a variable is added to the context, it should only be allowed to");
  doc("be added once. When the same data is added to the context, nothing");
  doc("should be added.");

  int result;
  int data = 34922;
  MGContext context;

  result = mg_init_context(&context);

  assert(mg_protect(&context, &data, sizeof(data)) == 1, "The first time variable is added, it should report as one.");
  assert(mg_protect(&context, &data, sizeof(data)) == 0, "When trying to add again, we should get one again.");
}

TEST(mg_protect_does_not_increase_size)
{
  doc("When the first variable is added to a context the size of the array");
  doc("should be correct. The size of the array should be one (1). We are");
  doc("ensuring that the default count of one (1) is preserved during the");
  doc("first variable being added.");

  int result;
  int data = 34922;
  MGContext context;

  result = mg_init_context(&context);
  result = mg_protect(&context, &data, sizeof(data));

  assert(context.size == 1, "The context size should be one (1).");
}

TEST(mg_protect_increases_count)
{
  doc("When additional variables are added to a context after the first then");
  doc("the count should start increasing.");

  int result;
  int data0 = 31337;
  int data1 = 34922;
  int data2 = 11350;
  int data3 = 11550;
  MGContext context;

  result = mg_init_context(&context);

  assert(result == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data0, sizeof(data0)) == 1, "Adding data0 should set count to 1.");
  assert(mg_protect(&context, &data1, sizeof(data1)) == 2, "Adding data1 should set count to 2.");
  assert(mg_protect(&context, &data2, sizeof(data2)) == 3, "Adding data2 should set count to 3.");
  assert(mg_protect(&context, &data3, sizeof(data3)) == 4, "Adding data3 should set count to 4.");
}

TEST(mg_protected_p_returns_position_if_protected)
{
  doc("When a variable is being passed to the function and it is protected");
  doc("then the array position is returned. So, any number greater than 0");
  doc("means the variable is protected.");

  int data, snafu = 31337;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data, sizeof(data)) == 1, "The variable 'data' should be added to context as protected.");
  assert(mg_protected_p(&context, &data) == 0, "The variable 'data' should be at array position 0.");
}

TEST(mg_protected_p_returns_negative_if_not_protected)
{
  doc("When a variable is being passed and it is not protected then the result");
  doc("will be negative one (-1).");

  int data0 = 31337;
  int data1 = 34922;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data0, sizeof(data0)) == 1, "The variable 'data0' should be added to context as protected.");
  assert(mg_protected_p(&context, &data1) == -1, "The variable 'data1' should not be protected.");
}

TEST(mg_update_creates_new_hash)
{
  doc("When updating a variable's value a new hash is created.");

  int data = 31337;
  unsigned int old_hash, new_hash;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data, sizeof(data)) == 1, "The variable 'data' should be added to the context as protected.");

  data = 34922;
  new_hash = mg_hash((char *)&data, sizeof(data));
  old_hash = context.array[0].hash;

  assert(mg_update(&context, &data) == sizeof(data), "Updating 'data' in context should return size of data.");
  assert(context.array[0].hash == new_hash, "The hash value within the context should be updated.");
  assert(context.array[0].hash != old_hash, "The new hash should not be the same as the old hash value.");
}

TEST(mg_update_returns_size)
{
  doc("When an update is successful the size of the data is returned.");

  int data = 31337;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data, sizeof(data)) == 1, "Variable 'data' should be added to the context as protected.");

  data = 34922;

  assert(mg_update(&context, &data) == sizeof(data), "The size of 'data' should be returned when updating.");
}

TEST(mg_update_returns_nothing_if_not_protected)
{
  doc("When an update is attempted and the variable passed is not protected");
  doc("then zero (0) is returned.");

  int data0 = 31337;
  int data1 = 34922;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data0, sizeof(data0)) == 1, "Variable 'data0' should be added to the context as protected.");

  assert(mg_update(&context, &data1) == 0, "Return zero since 'data1' is not protected within context.");
}

TEST(mg_compromised_p_returns_zero_if_not_compromised)
{
  doc("When checking if data has been compromised return zero (0) if the data");
  doc("has not been compromised and is secure.");

  int data = 34922;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data, sizeof(data)) == 1, "Variable 'data' should be added to the context as protected.");
  assert(mg_compromised_p(&context, &data) == 0, "Should return zero (0) since data is not compromised.");
}

TEST(mg_compromised_p_returns_one_if_compromised)
{
  doc("When checking if data has been compromised return one (1) if the data");
  doc("has in fact been compromised.");

  int data = 31337;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data, sizeof(data)) == 1, "Variable 'data' should be added to the context as protected.");

  data = 34922;

  assert(mg_compromised_p(&context, &data) == 1, "Data has been compromised, should return one (1).");
}

TEST(mg_compromised_p_returns_one_if_not_protected)
{
  int data0 = 31337;
  int data1 = 34922;
  MGContext context;

  assert(mg_init_context(&context) == 1, "Context should init correctly.");
  assert(mg_protect(&context, &data0, sizeof(data0)) == 1, "Variable 'data0' should be added to the context as protected.");

  data0 = 34922;

  assert(mg_compromised_p(&context, &data1) == 1, "Treat data not being protected as compromised.");
}

struct bank_account {
  char account_number[16];
  int balance;
};

struct bank_transfer {
  char from_account_number[16];
  char to_account_number[16];
  int amount;
};

TEST(mg_stress_test)
{
  doc("This is a light, fast moving stress test using two different MGContexts.");
  doc("The test is to test library functions when they are called many times");
  doc("quickly without any memory or other errors.");

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

  assert(mg_init_context(&mgc_accounts) == 1, "Stress test accounts context should init.");
  assert(mg_init_context(&mgc_transfers) == 1, "Stress test transfers context should init.");

  for (int i = 0; i < 128; i++)
    mg_protect(&mgc_accounts, &accounts[i], sizeof(accounts[i]));

  for (int i = 0; i < 1024; i++)
    mg_protect(&mgc_transfers, &transfers[i], sizeof(transfers[i]));

  for (int i = 0; i < 1024; i++) {
    int from_acct = atoi(transfers[i].from_account_number);
    int to_acct = atoi(transfers[i].to_account_number);

    if (i == 2 || i == 4 || i == 8 || i == 16 || i == 32 || i == 64) {
      accounts[i].balance = accounts[i].balance + 100;

      assert(mg_compromised_p(&mgc_accounts, &accounts[i]) > -1, "Account should be compromised.");
      assert(mg_update(&mgc_accounts, &accounts[i]) == sizeof(accounts[i]), "Update accounts.");
    }

    if (!mg_compromised_p(&mgc_accounts, &accounts[from_acct]) &&
        !mg_compromised_p(&mgc_accounts, &accounts[to_acct]) &&
        !mg_compromised_p(&mgc_transfers, &transfers[i])) {

      accounts[from_acct].balance = accounts[from_acct].balance - transfers[i].amount;
      accounts[to_acct].balance = accounts[to_acct].balance + transfers[i].amount;

      assert(mg_update(&mgc_accounts, &accounts[from_acct]) == sizeof(accounts[from_acct]), "Update from_acct.");
      assert(mg_update(&mgc_accounts, &accounts[to_acct]) == sizeof(accounts[to_acct]), "Update to_acct.");
    }
  }
}

int main()
{
  RUN_TEST(mg_stress_test);

  RUN_TEST(mg_hash_returns_a_hash);
  RUN_TEST(mg_hash_same_data_same_hash);
  RUN_TEST(mg_hash_changes_with_data);
  RUN_TEST(mg_hash_changes_with_any_data);

  RUN_TEST(mg_init_context_sets_up_context);
  RUN_TEST(mg_init_context_can_create_more_than_one);

  RUN_TEST(mg_protect_adds_variable_to_context);
  RUN_TEST(mg_protect_does_not_add_existing_variable);
  RUN_TEST(mg_protect_does_not_increase_size);
  RUN_TEST(mg_protect_increases_count);
  RUN_TEST(mg_protected_p_returns_position_if_protected);
  RUN_TEST(mg_protected_p_returns_negative_if_not_protected);

  RUN_TEST(mg_update_creates_new_hash);
  RUN_TEST(mg_update_returns_size);
  RUN_TEST(mg_update_returns_nothing_if_not_protected);

  RUN_TEST(mg_compromised_p_returns_zero_if_not_compromised);
  RUN_TEST(mg_compromised_p_returns_one_if_compromised);
  RUN_TEST(mg_compromised_p_returns_one_if_not_protected);

  PRINT_STATS();

  return 0;
}
