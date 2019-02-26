struct fname_entry{
      char* fname;
      struct fname_entry* next, * first, * last;
};

struct fname{
      int bux, n, * in_use;
      struct fname_entry** filenames;
};

struct fname* fname_init(struct fname* fn, int buckets);
void add_file_to_fhash(ino_t file_no, char* fname);
char* get_fname(struct fname* fn, ino_t file_no);
void fhash_free(struct fname* fn);
