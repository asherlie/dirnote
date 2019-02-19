#include <sys/stat.h>
#include <dirent.h>

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

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct fsys* fsys_build(struct fsys* fs, char* fpath);
struct finf** fsys_cmp(struct fsys* fs0, struct fsys* fs1, int* ret_sz);
void fsys_merge(struct fsys* fs_dest, struct fsys* fs_src);

/* track_changes should not be called without pthread_create */
void track_changes(struct tc_arg* tca);
