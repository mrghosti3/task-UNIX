#ifndef __STUDENT_TREE
#define __STUDENT_TREE

#include "libs/tree.h"

#include "student.h"

// Represents student list entry in RB Tree
struct node {
  RB_ENTRY(node) entry;
  struct slist_stud slist;
};

#define INIT_NODE(student_list) (struct node){{NULL,NULL,NULL}, SLIST_HEAD_INITIALIZER(student_list)}
struct node *init_node(struct student *stud);
// function that compares nodes
int nodecmp(struct node *n1, struct node *n2);

RB_HEAD(studtree, node);

RB_PROTOTYPE(studtree, node, entry, nodecmp);

void free_tree_nodes(struct studtree *stree);
struct studtree instantiate_tree(struct slist_stud *snames);
struct node *search(struct studtree *stree, struct student *s);
struct node *delete(struct studtree *stree, struct node *n);
void traverse(struct studtree *stree);

#endif // !__STUDENT_TREE
