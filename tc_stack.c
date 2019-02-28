#include "tc_stack.h"
#include <stdlib.h>

struct tc_stack* tc_stack_init(struct tc_stack* tcs){
      if(!tcs)tcs = malloc(sizeof(struct tc_stack));
      tcs->n = 0;
      tcs->top = NULL;
      return tcs;
}

void tc_stack_push(struct tc_stack* tcs, ino_t file_no, int alt_type){
      // if first item
      struct tc_stack_entry* cur = NULL;
      pthread_mutex_lock(&tcs->tc_stack_mut);
      if(!tcs->top){
            cur = tcs->top = malloc(sizeof(struct tc_stack_entry));
            cur->prev = NULL;
      }
      else{
            cur = tcs->top->next = malloc(sizeof(struct tc_stack_entry));
            cur->prev = tcs->top;
            tcs->top = cur;
      }
      cur->file_no = file_no;
      cur->alt_type = alt_type;
      cur->next = NULL;
      pthread_mutex_unlock(&tcs->tc_stack_mut);
}

struct tc_stack_entry* tc_stack_pop(struct tc_stack* tcs){
      struct tc_stack_entry* ret = tcs->top;
      if(!ret)return NULL;
      tcs->top = ret->prev;
      tcs->top->next = NULL;
      return ret;
}

void tc_stack_free(struct tc_stack* tcs){
      // this should leverage tc_stack_pop
      struct tc_stack_entry* tcse;
      while((tcse = tc_stack_pop(tcs)))free(tcse);
}
