#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XXH_INLINE_ALL

#include "libs/queue.h"
#include "libs/tree.h"
#include "libs/xxhash.h"

#define MEM_ALLOC_FAIL(ptr, stat) if (ptr == NULL) _exit(stat)
#define MEM_ALLOC_FAIL_RET(ptr, ret) if (ptr == NULL) return ret

struct name {
    char *name;
    size_t len;
};

inline void free_name(struct name n)
{ free(n.name); }

struct student {
    struct names *sn;
    SLIST_ENTRY(student) classmates;
};

// Student list functions:
SLIST_HEAD(slist_stud, student);

struct slist_stud instantiate_slist(int sage[], int sagec)
{
    struct slist_stud head = SLIST_HEAD_INITIALIZER(head);
    SLIST_INIT(&head);

    struct student *ps, *s = malloc(sizeof(struct student));
    MEM_ALLOC_FAIL_RET(s, head);

    // TODO: Replace age with names.
    /* s->age = sage[0]; */

    SLIST_INSERT_HEAD(&head, s, classmates);

    for (int i = 1; i < sagec; ++i)
    {
        ps = s;
        s = malloc(sizeof(struct student));
        MEM_ALLOC_FAIL_RET(s, head);

        // TODO: Replace age with names.
        /* s->age = sage[i]; */
        SLIST_INSERT_AFTER(ps, s, classmates);
    }

    return head;
}

void free_slist(struct slist_stud head) {
    struct student *s;

    while (!SLIST_EMPTY(&head))
    {
        s = SLIST_FIRST(&head);
        SLIST_REMOVE_HEAD(&head, classmates);
        free(s);
    }
}

// File reading part

/**
 * Retrieves array of names from given file.
 */
int get_names(struct name **names, const char *fname)
{
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return 0;

    struct stat fst;

    if (fstat(fd, &fst)) return 0;

    char *buff = malloc(fst.st_size);
    MEM_ALLOC_FAIL_RET(buff, 0);
    read(fd, buff, fst.st_size);
    close(fd);

    size_t off = 0;
    int c = 0, namec = 5;
    *names = calloc(sizeof(struct name), namec);

    for (size_t i = 0; i < fst.st_size; ++i)
    {
        if (buff[i] != '\n') continue;

#define cname(curr) (*names)[curr]
        cname(c).len = ++i - off;
        cname(c).name = malloc(cname(c).len);
        memcpy(cname(c).name, buff + off, cname(c).len);
        cname(c).name[cname(c).len - 1] = 0;
#undef cname
        off = i;
        ++c;
    }
    free(buff);

    if (c <= namec) *names = realloc(*names, sizeof(struct name) * c);
    return c;
}

int main(int argc, char *argv[])
{
    if (argc > 2) return -1;

    int snamec = 0;
    struct name *snames = NULL;

    snamec = get_names(&snames, argv[1]);

    MEM_ALLOC_FAIL(snames, EXIT_FAILURE);
    else if (!snamec)
    {
        printf("Empty input data");
        _exit(EXIT_SUCCESS);
    }

    /* struct slist_stud head = instantiate_slist(sage, sagec); */

    /* if (SLIST_EMPTY(&head)) { */
    /*     return -1; */
    /* } */

    /* free_slist(head); */

    for (int i = 0; i < snamec; ++i)
    {
        printf("OUTPUT: i %d len %zu '%s'\n", i, snames[i].len, snames[i].name);
        free(snames[i].name);
    }
    free(snames);
    return 0;
}
