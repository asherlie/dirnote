#include <stdlib.h>
#include <string.h>

#include "fname_hash.h"

struct fname* fname_init(struct fname* fn, int buckets){
      if(!fn)fn = malloc(sizeof(struct fname));
      fn->bux = buckets;
      fn->filenames = malloc(sizeof(struct fname_entry*)*fn->bux);
      fn->in_use = malloc(sizeof(int)*(fn->bux+1));
      memset(fn->in_use, -1, sizeof(int)*(fn->bux+1));
      fn->n = 0;
      return fn;
}

void add_file_to_fhash(struct fname* fn, ino_t file_no, char* fname){
      int ind = file_no%fn->bux;
      struct fname_entry* cur;
      // if this is the first entry of bucket ind
      if(!fn->filenames[ind]){
            cur = fn->filenames[ind] = malloc(sizeof(struct fname_entry));
            fn->filenames[ind]->first = fn->filenames[ind]->last = cur;
            fn->in_use[fn->n++] = ind;
      }
      else cur = fn->filenames[ind]->last->next = malloc(sizeof(struct fname_entry));
      cur->next = NULL;
      cur->fname = fname;
      cur->file_no = file_no;
}

char* get_fname(struct fname* fn, ino_t file_no){
      int ind = file_no%fn->bux;
      for(struct fname_entry* fe = fn->filenames[ind]->first; fe; fe = fe->next){
            if(fe->file_no == file_no)return fe->fname;
      }
      return NULL;
}

void fhash_free(struct fname* fn){
      for(int i = 0; fn->in_use[i] != -1; ++i){
            free(fn->filenames[fn->in_use[i]]->fname);
            free(fn->filenames[fn->in_use[i]]);
      }
      free(fn->filenames);
}
