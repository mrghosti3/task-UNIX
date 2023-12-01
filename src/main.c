#include <unistd.h>

// queue.h Integrations
#include "libs/queue.h"
#include "student.h"

// tree.h integration
#include "student_tree.h"

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

  if (RB_EMPTY(&stree) && !SLIST_EMPTY(&snames)) {
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

  free_tree_nodes(&stree);
  return 0;
}
