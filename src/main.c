#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/queue.h"
#include "libs/tree.h"
#define XXH_INLINE_ALL
#include "libs/xxhash.h"

// queue.h Integrations

// NOTE: may be better to use student struct as entry on SLIST_ENTRY
struct student {
  char *name;
  size_t nlen;
  XXH64_hash_t h1, h2;
  SLIST_ENTRY(student) classmates;
};

SLIST_HEAD(slist_stud, student);

void free_slist_items(struct slist_stud *head) {
  while (!SLIST_EMPTY(head)) {
    struct student *s = SLIST_FIRST(head);
    SLIST_REMOVE_HEAD(head, classmates);
    free(s->name);
    free(s);
  }
}

// FIX: reimplement hash collission resolution to match described in README
struct student new_student(char *name, size_t len) {
  XXH64_hash_t h1 = XXH64(name, len, 0), h2 = 1 + XXH64(name, len, 0) % len;
  struct student s = {name, len, h1, h2, {NULL}};
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
  printf("Student: h1 '%lx' h2 '%3lx' len %zu '%s' \n", s->h1, s->h2, s->nlen,
         s->name);
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

// tree.h integration

struct node {
  RB_ENTRY(node) entry;
  struct student *s;
};

int studcmp(struct node *n1, struct node *n2) {
  struct student *s1 = n1->s, *s2 = n2->s;

  if (s1->h1 != s2->h1)
    return s1->h1 < s2->h1 ? -1 : 1;

  return (s1->h2 < s2->h2 ? -1 : s1->h2 > s2->h2);
}

RB_HEAD(studtree, node);
RB_PROTOTYPE(studtree, node, entry, studcmp);
RB_GENERATE(studtree, node, entry, studcmp);

void free_tree_nodes(struct studtree *stree) {
  struct node *n, *np;

  RB_FOREACH_SAFE(n, studtree, stree, np) {
    RB_REMOVE(studtree, stree, n);
    free(n);
  }
}

struct studtree instantiate_tree(struct slist_stud *snames) {
  struct studtree head = RB_INITIALIZER(root);

  struct node *n;
  struct student *s;
  SLIST_FOREACH(s, snames, classmates) {
    if ((n = malloc(sizeof(struct node))) == NULL) {
      free_tree_nodes(&head);
      goto ret;
    }

    n->s = s;
    RB_INSERT(studtree, &head, n);
  }

ret:
  return head;
}

struct node *search(struct studtree *stree, struct student *s) {
  struct node lookup = {{NULL}, s};

  return RB_FIND(studtree, stree, &lookup);
}

struct node *delete(struct studtree *stree, struct node *n) {
  return RB_REMOVE(studtree, stree, n);
}

// FIX: implement tree pretty print
void traverse(struct studtree *stree) {
  struct node *n;
  RB_FOREACH(n, studtree, stree) { print_stud(n->s); }

  printf("\n");
}

int main(int argc, char *argv[]) {
  // checks if input filename is provided
  if (argc > 2) {
    return -1;
  }

  // SLIST creation
  struct slist_stud snames = instantiate_slist(argv[1]);

  if (SLIST_EMPTY(&snames)) {
    _exit(EXIT_FAILURE);
  }

  // SLIST test
  print_slist(&snames);

  // tree creation
  struct studtree stree = instantiate_tree(&snames);

  if (RB_EMPTY(&stree)) {
    free_slist_items(&snames);
    _exit(EXIT_FAILURE);
  }

  RB_UP(RB_ROOT(&stree), entry) = NULL;
  traverse(&stree);

  struct student s = new_student("Arnas Vidžiūnas", 18);
  struct node *n = search(&stree, &s);
  delete (&stree, n);
  free(n);
  traverse(&stree);

  free_slist_items(&snames);
  free_tree_nodes(&stree);
  return 0;
}
