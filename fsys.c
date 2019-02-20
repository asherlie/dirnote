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

// returns an finf** with all updated files its size will be the size of fs_new
struct finf** fsys_cmp(struct fsys* fs_new, struct fsys* fs_old, int* ret_sz){
      struct finf** ret =  malloc(sizeof(struct finf*)*fs_new->n);
      *ret_sz = 0;
      /* need to check if:
       *    fs_new[i] is not in fs_old
       *    fs_new[i] is in fs_old but has a new edit_t
       *    fs_old will never have new elements
       *
       */
      _Bool ex, alt;
      struct finf* tmp_fi;

      for(int i = 0; i < fs_new->n; ++i){
            ex = alt = 0;
            for(int j = 0; j < fs_old->n; ++j){
                  if(fs_new->files[i].file_no == 
                     fs_old->files[j].file_no){
                        ex = 1;
                        if(fs_new->files[i].edit_t != fs_old->files[j].edit_t)
                              alt = 1;
                        break;
                  }
            }
            if((ex && alt) || !ex){
                  tmp_fi = malloc(sizeof(struct finf));
                  memcpy(tmp_fi, &fs_new->files[i], sizeof(struct finf));
                  ret[(*ret_sz)++] = tmp_fi;
            }
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
