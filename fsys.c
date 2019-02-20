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

struct finf finf_build(time_t edit_t, ino_t file_no, char* fname, unsigned char namelen){
      struct finf f;
      f.edit_t = edit_t;
      f.file_no = file_no;
      strncpy(f.fname, fname, (namelen > 0) ? namelen : 255);
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
                  #ifdef _DIRENT_HAVE_D_NAMLEN
                  fsys_insert(fs, finf_build(attr.st_mtime, dir->d_fileno, dir->d_name, dir->d_namlen));
                  #else
                  fsys_insert(fs, finf_build(attr.st_mtime, dir->d_fileno, dir->d_name, -1));
                  #endif
            }
            closedir(d);
      }
      else{
            if(m)free(fs);
            return NULL;
      }
      return fs;
}

// returns an finf** with all updated files its size will be the largest of fs0, fs1
// if a file is different in f1 than in f0, changes to f1 are printed
struct finf** fsys_cmp(struct fsys* fs0, struct fsys* fs1, int* ret_sz){
      // large and small
      struct fsys* l = fs0;
      struct fsys* s = fs1;

      if(fs0->n < fs1->n){
            s = fs0;
            l = fs1;
      }

      struct finf** ret =  malloc(sizeof(struct finf*)*l->n);
      
      /*
      for(int i = 0; i < l->n; ++i){
            for(int j = 0; j < s->n; ++j){
                  // check if l[i] is in s
                  // check if s[i] is in l
                  if()
            }
      }
      */

      *ret_sz = 0;

      struct finf* tmp_fi;

      _Bool ex, alt;
      for(int i = 0; i < l->n; ++i){
            // to keep track of existence, alteration
            ex = 0, alt = 0;
            for(int j = 0; j < s->n; ++j){
                  // if s contains l->files[i]
                  if(l->files[i].file_no == s->files[j].file_no){
                        ex = 1;
                        // TODO: keep track of larger time, set edit_t to 0
                        // in smaller so as not to find discrep again
                        alt = l->files[i].edit_t != s->files[j].edit_t;
                        // change this to be assigned to larger timeval
                        tmp_fi = malloc(sizeof(struct finf));
                        memcpy(tmp_fi, &l->files[i], sizeof(struct finf));
                        // tmp_fi = l->files[i];
                  }
            }
            // if(alt)... add larger time thing, will b saved in some shit
            // umm ... on the stack?...
            if(!ex || alt){
                  printf("inserting %i\n", *ret_sz);
                  ret[(*ret_sz)++] = tmp_fi;
            }
      }

      return ret;
}

struct finf** fsys_cmp_og(struct fsys* fs0, struct fsys* fs1, int* ret_sz){
      struct finf** ret = malloc(sizeof(struct finf*)*((fs0->n > fs1->n) ? fs0->n : fs1->n));
      *ret_sz = 0;
      _Bool new_f, add;
      for(int i = 0; i < fs0->n; ++i){
            new_f = 1;
            add = 0;
            for(int j = 0; j < fs1->n; ++j){
                  if(fs0->files[i].file_no == fs1->files[j].file_no){
                        new_f = 0;
                        if(fs0->files[i].edit_t != fs1->files[j].edit_t)add = 1;
                  }
            }
            if(new_f || add)
                  ret[(*ret_sz)++] = &fs0->files[i];
      }
      return ret;
}

// merges fs_src into fs_dest
// obsolete - we will always opt for creating a new struct fsys with fsys_build
void fsys_merge(struct fsys* fs_dest, struct fsys* fs_src){
      (void)fs_dest;
      (void)fs_src;
}

// resolution is time to sleep between checks in secs
// this is meant to be called from pthread_create()
void track_changes(struct tc_arg* tca){
      struct fsys* fs_o = fsys_build(NULL, tca->fpath);
      struct fsys* tmp_fs = malloc(sizeof(struct fsys));
      struct finf** cmp;
      int diff;
      while(usleep(tca->res*1e6) || tca->run){
            diff = 0;
            fsys_build(tmp_fs, tca->fpath);
            if((cmp = fsys_cmp(fs_o, tmp_fs, &diff)) && diff > 0){
                  // a file has been altered
                  printf("%i files have been altered\n", diff);
                  puts("those files:");
                  for(int i = 0; i < diff; ++i){
                        printf("%s @ %li\n", cmp[i]->fname, cmp[i]->file_no);
                        free(cmp[i]);
                  }
                  free(cmp);
                  // old fs should be updated
                  fs_o = tmp_fs;
            }
      }
}
