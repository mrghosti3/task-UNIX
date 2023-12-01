#include "student.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

void free_slist_items(struct slist_stud *head) {
  while (!SLIST_EMPTY(head)) {
    struct student *s = SLIST_FIRST(head);
    SLIST_REMOVE_HEAD(head, classmates);
    free(s->name);
    free(s);
  }
}

struct student new_student(char *name, size_t len) {
  XXH64_hash_t h1 = XXH64(name, len, 0);
  struct student s = {name, len, h1, {NULL}};
  return s;
}

struct student *stud_from_text(char *buff, size_t i, size_t off) {
  size_t len = i - off;
  char *name = malloc(len);
  if (name == NULL) {
    return NULL;
  }

  memcpy(name, buff + off, len);
  name[len - 1] = 0; // makes sure that string ends with '\0'

  struct student *s = malloc(sizeof(struct student));
  if (s == NULL) {
    free(name);
    return NULL;
  }

  *s = new_student(name, len);
  return s;
}

struct slist_stud instantiate_slist(const char *fname) {
  struct slist_stud head = SLIST_HEAD_INITIALIZER(&head);

  int fd = open(fname, O_RDONLY);
  if (fd < 0) {
    goto ret;
  }

  struct stat fst;
  if (fstat(fd, &fst)) {
    goto ret;
  }

  char *buff = mmap(NULL, fst.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (buff == MAP_FAILED) {
    goto ret;
  }

  SLIST_INIT(&head);

  size_t off = 0;
  for (size_t i = 0; i < fst.st_size; ++i) {
    if (buff[i] != '\n') {
      continue;
    }

    struct student *s = stud_from_text(buff, ++i, off);
    if (s == NULL) {
      // If failed to create student, it cleans up the data
      // and returns empty struct slist_stud.
      free_slist_items(&head);
      goto ret;
    }

    SLIST_INSERT_HEAD(&head, s, classmates);
    off = i;
  }
  munmap(buff, fst.st_size);

ret:
  close(fd);
  return head;
}

void print_stud(struct student *s) {
  printf("Student: h1 '%lx' len %zu '%s' \n", s->h1, s->nlen, s->name);
}

void print_slist(struct slist_stud *snames) {
  int i = 0;
  struct student *el;
  SLIST_FOREACH(el, snames, classmates) {
    printf("%d ", i++);
    print_stud(el);
  }

  printf("\n");
}
