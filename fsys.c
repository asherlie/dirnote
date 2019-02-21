#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>

#include "fsys.h"

void fsys_init(struct fsys* fs){
      fs->n = 0;
      fs->cap = 10;
      fs->files = malloc(sizeof(struct finf)*fs->cap);
}

struct finf finf_build(time_t edit_t, ino_t file_no, char* fname){
      struct finf f;
      f.edit_t = edit_t;
      f.file_no = file_no;
      strcpy(f.fname, fname);
      return f;
}

_Bool fsys_insert(struct fsys* fs, struct finf f){
      _Bool resize;
      if((resize = (fs->n == fs->cap))){
            fs->cap *= 2;
            struct finf* tmp_finf = malloc(sizeof(struct finf)*fs->cap);
            memcpy(tmp_finf, fs->files, sizeof(struct finf)*fs->n);
            free(fs->files);
            fs->files = tmp_finf;
      }
      fs->files[fs->n++] = f;
      return resize;
}

// if(!fs) a new fsys* is malloc'd
struct fsys* fsys_build(struct fsys* fs, char* fpath){
      _Bool m = 0;
      if(!fs){
            m = 1;
            fs = malloc(sizeof(struct fsys));
      }
      fsys_init(fs);
      struct stat attr;
      DIR* d = opendir(fpath);
      struct dirent* dir;
      if(d){
            while((dir = readdir(d))){
                  stat(dir->d_name, &attr);
                  fsys_insert(fs, finf_build(attr.st_mtime, dir->d_fileno, dir->d_name));
            }
            closedir(d);
      }
      else{
            if(m)free(fs);
            return NULL;
      }
      return fs;
}

struct fsys_cmp_in* fci_init(struct fsys_cmp_in* fci){
      if(!fci)fci = malloc(sizeof(struct fsys_cmp_in));
      fci->n = 0;
      fci->cap = 10;
      fci->fce = calloc(fci->cap, sizeof(struct fsys_cmp_entry));
      return fci;
}

void fce_add_inf(struct fsys_cmp_in* fci, ino_t key, time_t edit_t, int age){
      // indexing into fci->fce will be done with this hashing function
      // first check if index has correct info
      int ind;
      if(fci->fce[(ind = key%fci->cap)].key != key){
            fci->cap *= 2;
            struct fsys_cmp_entry* fce_tmp = calloc(fci->cap, sizeof(struct fsys_cmp_entry));
            memcpy(fce_tmp, fci->fce, fci->n*sizeof(struct fsys_cmp_entry));
            free(fci->fce);
            fci->fce = fce_tmp;
      }
      // insert into ind
      fci->fce[ind].edit_t[age] = edit_t;
      fci->fce[ind].key = key;
      if(age == NEW)fci->fce[ind].old = 1;
      else fci->fce[ind].new = 1;
      if(fci->fce[ind].old && fci->fce[ind].new && fci->fce[ind].edit_t[0] != fci->fce[ind].edit_t[1])
            fci->fce[ind].alt = 1;
      ++fci->n;
}

// returns a malloc'd struct fsys_cmp_in* comparing fs_new and fs_old
struct fsys_cmp_in* build_fci(struct fsys* fs_new, struct fsys* fs_old){
      struct fsys_cmp_in* ret = malloc(sizeof(struct fsys_cmp_in));
      fci_init(ret);
      // old entries must be added first
      for(int i = 0; i < fs_old->n; ++i)
            fce_add_inf(ret, fs_old->files[i].file_no, fs_old->files[i].edit_t, OLD);
      for(int i = 0; i < fs_new->n; ++i)
            fce_add_inf(ret, fs_new->files[i].file_no, fs_new->files[i].edit_t, NEW);
      return ret;
}

// returns a malloc'd struct fsys_cmp_in*
// returns NULL if no change detected
struct fsys_cmp_in* fsys_cmp(struct fsys* fs_new, struct fsys* fs_old, int* n_alt){
      struct fsys_cmp_in* fci = build_fci(fs_new, fs_old);
      *n_alt = 0;
      for(int i = 0; i < fci->n; ++i){
            if(fci->fce[i].alt || (fci->fce[i].new ^ fci->fce[i].old))
                  ++(*n_alt);
      }
      if(!fci->n || !*n_alt){
            free(fci->fce);
            free(fci);
            fci = NULL;
      }
      return fci;
}

// resolution is time to sleep between checks in secs
// this is meant to be called from pthread_create()
void track_changes(struct tc_arg* tca){
      struct fsys* fs_o = fsys_build(NULL, tca->fpath);
      struct fsys* tmp_fs = malloc(sizeof(struct fsys));

      struct fsys_cmp_in* cmp;
      int diff;
      while(usleep(tca->res*1e6) != -1 && tca->run){
            diff = 0;
            fsys_build(tmp_fs, tca->fpath);
            if((cmp = fsys_cmp(fs_o, tmp_fs, &diff))){
                  printf("%i files have been altered\n", diff);
                  free(cmp);
                  // old fs should be updated
                  fs_o = tmp_fs;
            }
      }
}
