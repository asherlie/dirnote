#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>

#include "fname_hash.h"

#define OLD 0
#define NEW 1

// creation, deletion, change
#define ALT_CRE 0
#define ALT_DEL 1
#define ALT_CHA 2

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

struct finf{
      time_t edit_t;
      ino_t file_no;
};

struct fsys{
      int n, cap;
      struct finf* files;
};
/* this is an argument struct for track_changes */

struct tc_arg{
      char* fpath;
      int res;
      // once *run == 0, tc will safely exit
      _Bool* run;

      struct tc_stack* tc_stack;
};

// TODO: this struct is obsolete
struct track_chng{
      _Bool* run;

      struct tc_arg* tca;

      struct fname* fname_hash;

      pthread_t pth;
      // this is a pointer to the tc_stack in tc_arg for ease of use
      struct tc_stack* tc_stack;
};

struct fsys_cmp_entry{
      ino_t key;
      // old, new
      time_t edit_t[2];
      // if this file exists in old, new
      _Bool old, new, alt;
      int alt_type;
      struct fsys_cmp_entry* first, * last, * next;
};

struct fsys_cmp_in{
      // hashed `ino_t`s point to linked lists of cmp_entries
      // bucket_ind stores all the populated bucket indices
      int n, bux, * bucket_ind;
      struct fsys_cmp_entry* cmp_entries;
};

struct fsys_cmp_in* fci_init(struct fsys_cmp_in* fci);

// age can be passed OLD or NEW
// age is recorded as having file key once this is called
void fce_add_inf(struct fsys_cmp_in* fci, ino_t key, time_t edit_t, int age);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct fsys* fsys_build(struct fsys* fs, char* fpath);
//struct finf** fsys_cmp(struct fsys* fs0, struct fsys* fs1, int* ret_sz);
// returns a malloc'd struct fsys_cmp_in*
// returns NULL if no change detected
struct fsys_cmp_in* fsys_cmp(struct fsys* fs_new, struct fsys* fs_old, int* n_alt);
void fsys_merge(struct fsys* fs_dest, struct fsys* fs_src);

void fsys_free(struct fsys* fs);
/* track_changes should not be called without pthread_create */
// returns a pointer to _Bool run in tc_arg
struct track_chng track_changes(char* fpath, int res);
void untrack_changes(struct track_chng tc);
