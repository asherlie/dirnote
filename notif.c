#include <stdio.h>
#include "fsys.h"

int main(int argc, char** argv){
      if(argc == 1)return 1;
      struct tc_arg tca;
      tca.run = 1;
      tca.res = 1;
      tca.fpath = argv[1];
      track_changes(&tca);

      return 0;

      struct fsys* fs = fsys_build(NULL, argv[1]);
      while(getchar() != '\n');
      struct fsys* fs0 = fsys_build(NULL, argv[1]);
      int diff;
      fsys_cmp(fs0, fs, &diff);
      printf("diff size of %i\n", diff);
      return 0;
}
