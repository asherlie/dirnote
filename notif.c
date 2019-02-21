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
}
