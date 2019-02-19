#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

struct finf{
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

_Bool checker(char* fpath){
      struct stat attr; // attr.st_mtime 
      DIR* d = opendir(fpath);
      struct dirent* dir;
      if(d){
            while((dir = readdir(d))){
                  printf("%s\n", dir->d_name);
            }
            closedir(d);
      }
      else return 1;
      return 0;
}

int main(int argc, char** argv){
}
