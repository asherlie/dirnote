#include <sys/stat.h>
#include <dirent.h>

#define OLD 0
#define NEW 1

struct finf{
      char fname[255];
      unsigned char namelen;
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
      // once run == 0, tc will safely exit
      _Bool run;
};

struct fsys_cmp_entry{
      ino_t key;
      // if this file exists in old, new
      _Bool old, new, alt;
};

struct fsys_cmp_in{
      struct fsys_cmp_entry* fce;
      int n, cap;
};

struct fsys_cmp_in* fci_init(struct fsys_cmp_in* fci);

// age can be passed OLD or NEW
// age is recorded as having file key once this is called
void fce_add_inf(struct fsys_cmp_in* fci, ino_t key, int age);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct fsys* fsys_build(struct fsys* fs, char* fpath);
//struct finf** fsys_cmp(struct fsys* fs0, struct fsys* fs1, int* ret_sz);
// returns a malloc'd struct fsys_cmp_in*
// returns NULL if no change detected
struct fsys_cmp_in* fsys_cmp(struct fsys* fs_new, struct fsys* fs_old, int* n_alt);
void fsys_merge(struct fsys* fs_dest, struct fsys* fs_src);

/* track_changes should not be called without pthread_create */
void track_changes(struct tc_arg* tca);
