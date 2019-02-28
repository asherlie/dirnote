#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

/* TC_STACK */

// functions for initializing and freeing
// these structures are hidden

struct tc_stack_entry{
      ino_t file_no;
      int alt_type;
      struct tc_stack_entry* next, * prev;
};

struct tc_stack{
      int n;
      struct tc_stack_entry* top;
      pthread_mutex_t tc_stack_mut;
};

struct tc_stack* tc_stack_init(struct tc_stack* tcs);
void tc_stack_push(struct tc_stack* tcs, ino_t file_no, int alt_type);
// the tc_stack_entry returned from this function must be freed
struct tc_stack_entry* tc_stack_pop(struct tc_stack* tcs);
void tc_stack_free(struct tc_stack* tcs);

/* TC_STACK */

