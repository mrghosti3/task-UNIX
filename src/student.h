#ifndef __STUDENT
#define __STUDENT

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "libs/queue.h"
#define XXH_INLINE_ALL
#include "libs/xxhash.h"

// NOTE: may be better to use student struct as entry on SLIST_ENTRY
struct student {
  char *name;
  size_t nlen;
  XXH64_hash_t h1;
  SLIST_ENTRY(student) classmates;
};

SLIST_HEAD(slist_stud, student);

void free_slist_items(struct slist_stud *head);
struct student new_student(char *name, size_t len);
struct student *stud_from_text(char *buff, size_t i, size_t off);
struct slist_stud instantiate_slist(const char *fname);
void print_stud(struct student *s);
void print_slist(struct slist_stud *snames);

#endif // !__STUDENT
