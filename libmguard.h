#include <stdlib.h>

typedef struct MGProtectedVariable {
  void *ptr;
  size_t size;
  unsigned int hash;
} MGProtectedVariable;

typedef struct MGContext {
  struct MGProtectedVariable *array;
  size_t used;
  size_t size;
} MGContext;

int mg_init_context(MGContext *context);
size_t mg_protect(MGContext *context, void *data, size_t size);
size_t mg_update(MGContext *context, void *data);
int mg_compromised_p(MGContext *context, void *data);
int mg_protected_p(MGContext *context, void *data);

unsigned int mg_hash(char *data, size_t len);
