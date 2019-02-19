#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void fsys_init(struct fsys* fs){
      fs->n = 0;
      fs->cap = 10;
      fs->files = malloc(sizeof(struct finf)*fs->cap);
}

struct finf finf_build(time_t edit_t, ino_t file_no, char* fname, unsigned char namelen){
      struct finf f;
      f.edit_t = edit_t;
      f.file_no = file_no;
      strncpy(f.fname, fname, namelen);
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
      struct stat attr; // attr.st_mtime 
      DIR* d = opendir(fpath);
      struct dirent* dir;
      if(d){
            while((dir = readdir(d))){
                  stat(dir->d_name, &attr);
                  fsys_insert(fs, finf_build(attr.st_mtime, dir->d_fileno, dir->d_name, dir->d_namlen));
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
struct finf** fsys_cmp(struct fsys* fs0, struct fsys* fs1, int* ret_sz){
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
                  ret[*ret_sz++] = &fs0->files[i];
      }
      return ret;
}

// merges fs_src into fs_dest
void fsys_merge(struct fsys* fs_dest, struct fsys* fs_src){
      (void)fs_dest;
      (void)fs_src;
}

int main(int argc, char** argv){
      if(argc == 1)return 1;
      struct fsys* fs = fsys_build(NULL, argv[1]);
      int diff;
      fsys_cmp(fs, fs, &diff);
      printf("diff size of %i\n", diff);
      return 0;
}
