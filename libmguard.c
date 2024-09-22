#include <stddef.h>
#include <stdlib.h>
#include "libmguard.h"

/*
 * mg_init_context
 *
 * MGContext *context - Pointer to a memory guard context.
 *
 * This function initializes a memory guard context. The initialization
 * consists of allocating memory to the context array and setting the
 * counters.
 *
 */
int
mg_init_context(MGContext *context)
{
  context->array = malloc(sizeof(MGProtectedVariable));
  context->used = 0;
  context->size = 1;

  return context->size;
}

/*
 * mg_protect
 *
 * MGContext *context - Pointer to a memory guard context.
 * void      *data    - Pointer to data to protect.
 * size_t    size     - Size of data.
 *
 * This function protects the passed in data by adding it to the supplied
 * context. This is accomplished by creating a new MGProtectedvariable, then
 * hashing the data.
 *
 * Before doing anything, the data passed to protect is checked to see if it
 * is already being protected. If it is, then nothing is done and zero (0) is
 * returned. If the data is not being protected, then the size of the context
 * is returned.
 *
 */
size_t
mg_protect(MGContext *context, void *data, size_t size)
{
  MGProtectedVariable protected_variable;
  unsigned int hash = mg_hash((char *)data, size);

  if (mg_protected_p(context, data) == -1) {
    protected_variable.ptr = data;
    protected_variable.size = size;
    protected_variable.hash = hash;

    // TODO: Maybe this should be its own function.
    if (context->used == context->size) {
      context->size += 1;
      context->array = realloc(context->array,
                               context->size * sizeof(MGProtectedVariable));
    }

    context->array[context->used++] = protected_variable;
    // END of TODO note.

    return context->size;
  }

  return 0;
}

/*
 * mg_update
 *
 * MGContext *context - Pointer to a memory guard context.
 * void      *data    - Pointer to protected data.
 *
 * This function updates the hash of data that is already protected. If the data
 * supplied is actually protected, then the size is returned. If the data is not
 * protected, then zero (0) is returned.
 *
 */
size_t
mg_update(MGContext *context, void *data)
{
  int n;

  if ((n = mg_protected_p(context, data)) != -1) {
    context->array[n].hash = mg_hash((char *)data, context->array[n].size);
    return context->array[n].size;
  }

  return 0;
}

/*
 * mg_compromised_p
 *
 * MGContext *context - Pointer to a memory guard context.
 * void      *data    - Pointer ot protected data.
 *
 * This function is used to check if protected data has been compromised. The
 * check is conducted by creating a new hash of the passed in data and comparing
 * it with the hash stored in the context. If the hashes match then one (1) is
 * returned indicating the data is still protected. If the hashes do not match
 * then zero (0) is returned indicating the data has been compromised.
 *
 * If the data passed in to check is not protected, then it will be indicated
 * that the data has been compromised. So, if the data is not protected to start
 * with, then zero (0) will be returned.
 *
 */
int
mg_compromised_p(MGContext *context, void *data)
{
  int n;

  if ((n = mg_protected_p(context, data)) != -1) {
    unsigned int hash = mg_hash((char *)data, context->array[n].size);

    if (hash == context->array[n].hash)
      return 0;
  }

  return 1;
}

/*
 * mg_protected_p
 *
 * MGContext *context - Pointer to a memory guard context.
 * void      *data    - Pointer to data to check on.
 *
 * This function checks to see if the passed in data is being protected by
 * memory guard. This is done by checking the array located inside of the
 * passed in memory guard context for the data pointer. If the pointer is
 * found, then the array position is returned. If the data is not protected
 * then -1 is returned.
 *
 */
int
mg_protected_p(MGContext *context, void *data)
{
  for (int n = 0; n < context->used; n++)
    if (context->array[n].ptr == data)
      return n;

  return -1;
}

/*
 * mg_hash
 *
 * char   *data - Pointer to the data to hash.
 * size_t len   - Length of the data.
 *
 * This function creates a hash from the data passed in from the data pointer.
 * The hashing function is simple and is only needed to detect any changes in
 * the data.
 *
 * Example:
 *
 *   int account_number = 31337;
 *   mg_hash(&account_number, sizeof(account_number));
 *
 */
unsigned int
mg_hash(char *data, size_t len)
{
  unsigned int hash = 0;

  for (int n = 0; n < len; n++)
    hash = data[n] + (hash << 6) + (hash << 16) - hash;

  return hash;
}
