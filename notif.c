#include <stdio.h>
#include "fsys.h"

int main(int argc, char** argv){
      if(argc == 1)return 1;
      struct track_chng tc = track_changes(argv[1], 100);
      getchar();
      untrack_changes(tc);
      return 0;
}
