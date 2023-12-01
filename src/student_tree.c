#include "student_tree.h"
#include "libs/queue.h"
#include "student.h"

#include <stdio.h>

RB_GENERATE(studtree, node, entry, nodecmp);

struct node *init_node(struct student *stud) {
  struct node *n;
  if ((n = malloc(sizeof(struct node))) == NULL) {
    return NULL;
  }

  n->slist = (struct slist_stud)SLIST_HEAD_INITIALIZER(stud);
  SLIST_INSERT_HEAD(&n->slist, stud, classmates);
  return n;
}

int nodecmp(struct node *n1, struct node *n2) {
  XXH64_hash_t h1 = SLIST_FIRST(&n1->slist)->hash;
  XXH64_hash_t h2 = SLIST_FIRST(&n2->slist)->hash;

  return h1 < h2 ? -1 : h1 > h2;
}

void free_tree_nodes(struct studtree *stree) {
  struct node *n, *np;

  RB_FOREACH_SAFE(n, studtree, stree, np) {
    RB_REMOVE(studtree, stree, n);
    free(n);
  }
}

struct studtree instantiate_tree(struct slist_stud *stud_names) {
  struct studtree head = RB_INITIALIZER(root);

  while (!SLIST_EMPTY(stud_names)) {
    struct student *s = SLIST_FIRST(stud_names);
    SLIST_REMOVE_HEAD(stud_names, classmates);

    struct node *n = init_node(s);
    if (n == NULL) {
      free_tree_nodes(&head);
      goto ret;
      ;
    }

    RB_INSERT(studtree, &head, n);
  }

ret:
  return head;
}

// FIX: Rework search. Not vaiable to use RB_FIND
struct node *search(struct studtree *stree, struct student *s) {
  struct node lookup = INIT_NODE(&NULL);

  return RB_FIND(studtree, stree, &lookup);
}

struct node *delete(struct studtree *stree, struct node *n) {
  return RB_REMOVE(studtree, stree, n);
}

// FIX: Rework search. Not vaiable to use RB_FIND.
//
// FIX: implement tree pretty print
void traverse(struct studtree *stree) {
  struct node *n;
  RB_FOREACH(n, studtree, stree) {
    print_slist(&n->slist);
  }

  printf("\n");
}
