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
      if(!fs)fs = malloc(sizeof(struct fsys));
      struct stat attr; // attr.st_mtime 
      DIR* d = opendir(fpath);
      struct dirent* dir;
      if(d){
            while((dir = readdir(d))){
                  stat(dir->d_name, &attr);
                  printf("%s\n", dir->d_name);
                  fsys_insert(fs, finf_build(attr.st_mtime, dir->d_fileno, dir->d_name, dir->d_namlen));
            }
            closedir(d);
      }
      else return NULL;
      return fs;
}

int fsys_cmp(struct fsys* fs0, struct fsys* fs1){
      (void)fs0;
      (void)fs1;
      return 0;
}

// merges fs_src into fs_dest
void fsys_merge(struct fsys* fs_dest, struct fsys* fs_src){
      (void)fs_dest;
      (void)fs_src;
}

int main(int argc, char** argv){
      return 0;
}
