#include <stdio.h>
#include <stdlib.h>
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
      if(fs->n == fs->cap){
      }
      fs->files[fs->n++] = f;
}

// if(!fs) a new fsys* is malloc'd
struct fsys* fsys_build(struct fsys* fs, char* fpath){
      if(!fs)fs = malloc(sizeof(struct fsys));
      struct stat attr; // attr.st_mtime 
      DIR* d = opendir(fpath);
      struct dirent* dir;
      if(d){
            while((dir = readdir(d))){
                  printf("%s\n", dir->d_name);
            }
            closedir(d);
      }
      else return NULL:
      return fs
}

int fsys_cmp(struct fsys* fs0, struct fsys* fs1){
}

// merges fs_src into fs_dest
void fsys_merge(struct fsys* fs_dest, struct fsys* fs_src){
}

int main(int argc, char** argv){
}
