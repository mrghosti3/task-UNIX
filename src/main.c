#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/queue.h"
#define XXH_INLINE_ALL
#include "libs/xxhash.h"
#include "libs/tree.h"

// queue.h Integrations

// NOTE: may be better to use student struct as entry on SLIST_ENTRY
// FIX: implement double-hashing for hash collision resolution
struct student {
    char *name;
    size_t nlen;
    XXH64_hash_t hash;
    SLIST_ENTRY(student) classmates;
};

SLIST_HEAD(slist_stud, student);

struct student* create_student(char *buff, size_t i, size_t off)
{
    struct student *s = malloc(sizeof(struct student));
    if (s == NULL) return NULL;

    s->nlen = i - off;
    s->name = malloc(s->nlen);
    if (s->name == NULL)
    {
        free(s);
        return NULL;
    }

    memcpy(s->name, buff + off, s->nlen);
    s->name[s->nlen - 1] = 0;
    s->hash = XXH64(s->name, s->nlen, 0);
    return s;
}

struct slist_stud* instantiate_slist(const char *fname)
{
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return NULL;

    struct stat fst;

    if (fstat(fd, &fst)) return NULL;

    char *buff = mmap(NULL, fst.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buff == MAP_FAILED) return NULL;

    read(fd, buff, fst.st_size);
    close(fd);

    struct slist_stud *head = malloc(sizeof(struct slist_stud));
    if (head == NULL) return NULL;

    SLIST_INIT(head);

    size_t off = 0;
    struct student *ps;

    for (size_t i = 0; i < fst.st_size; ++i)
    {
        if (buff[i] != '\n') continue;

        struct student *s = create_student(buff, ++i, off);
        if (s == NULL) continue;

        SLIST_INSERT_HEAD(head, s, classmates);

        off = i;
        ps = s;
    }
    munmap(buff, fst.st_size);

    return head;
}

void print_slist(struct slist_stud *snames)
{
    int i = 0;
    struct student *el;
    SLIST_FOREACH(el, snames, classmates)
    {
        printf("OUTPUT: i %d len %zu '%s' '%zu'\n", i++, el->nlen, el->name, el->hash);
    }
}

void free_slist_items(struct slist_stud *head) {
    while (!SLIST_EMPTY(head))
    {
        struct student *s = SLIST_FIRST(head);
        SLIST_REMOVE_HEAD(head, classmates);
        free(s->name);
        free(s);
    }
}

// tree.h integration

struct node {
    RB_ENTRY(node) entry;
    struct student *s;
};

int studcmp(struct node *n1, struct node *n2)
{
    return (n1->s->hash < n2->s->hash ? -1 : n1->s->hash > n2->s->hash);
}

RB_HEAD(studtree, node);
RB_PROTOTYPE(studtree, node, entry, studcmp);
RB_GENERATE(studtree, node, entry, studcmp);

struct studtree* instantiate_tree(struct slist_stud *snames)
{
    struct studtree *head = malloc(sizeof(struct studtree));

    if (head == NULL) return NULL;

    struct node *n;
    struct student *s;
    SLIST_FOREACH(s, snames, classmates)
    {
        if ((n = malloc(sizeof(struct node))) == NULL)
        {
            return head;
        }

        n->s = s;
        RB_INSERT(studtree, head, n);
    }

    return head;
}

struct node* search(struct studtree *stree, struct student *s)
{
    struct node lookup = { { NULL }, s };

    return RB_FIND(studtree, stree, &lookup);
}

struct node* delete(struct studtree *stree, struct node *n)
{
    return RB_REMOVE(studtree, stree, n);
}

// FIX: implement tree pretty print
void traverse(struct studtree *stree)
{
    int i = 0;
    struct node *n;
    RB_FOREACH(n, studtree, stree)
    {
        struct student *el = n->s;
        printf("OUTPUT: i %d len %zu '%s' '%zu'\n", i++, el->nlen, el->name, el->hash);
    }
}

void free_tree_nodes(struct studtree *stree) {
    struct node *n, *np;

    RB_FOREACH_SAFE(n, studtree, stree, np)
    {
        RB_REMOVE(studtree, stree, n);
        free(n);
    }
}

int main(int argc, char *argv[])
{
    // checks if input filename is provided
    if (argc > 2) return -1;

    // SLIST creation
    struct slist_stud *snames = instantiate_slist(argv[1]);

    if (snames == NULL) _exit(EXIT_FAILURE);
    else if (SLIST_EMPTY(snames))
    {
        free(snames);
        _exit(EXIT_FAILURE);
    }

    // SLIST test
    print_slist(snames);

    // tree creation
    struct studtree *stree = instantiate_tree(snames);

    if (stree == NULL) {
        free_slist_items(snames);
        free(snames);
        _exit(EXIT_FAILURE);
    }
    else if (RB_EMPTY(stree))
    {
        free_slist_items(snames);
        free(snames);
        free(stree);
        _exit(EXIT_FAILURE);
    }

    RB_UP(RB_ROOT(stree), entry) = NULL;
    traverse(stree);

    free_slist_items(snames);
    free(snames);
    free_tree_nodes(stree);
    free(stree);
    return 0;
}
