#include <unistd.h>

// queue.h Integrations
#include "student.h"

// tree.h integration
#include <stdio.h>
#include "libs/tree.h"
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
