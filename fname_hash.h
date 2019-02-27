#include <dirent.h>
#include <sys/stat.h>
#ifndef _FNAME_HASH
#define _FNAME_HASH

struct fname_entry{
      char* fname;
      ino_t file_no;
      struct fname_entry* next, * last;
};

struct fname_lst{
      struct fname_entry* fnames, * first, * last;
};

struct fname{
      int bux, n, * in_use;
      struct fname_lst* filenames;
};

struct fname* fname_init(struct fname* fn, int buckets);
void add_file_to_fhash(struct fname* fn, ino_t file_no, char* fname);
char* get_fname(struct fname* fn, ino_t file_no);
// fhash_free frees all strings in fn->filenames[ind].fnames->fname
void fhash_free(struct fname* fn);
#endif
