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
#include "libs/tree.h" // TODO: implement tree usage

#define MEM_ALLOC_FAIL_RET(ptr, ret) if (ptr == NULL) return ret

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
    MEM_ALLOC_FAIL_RET(buff, NULL);
    read(fd, buff, fst.st_size);
    close(fd);

    struct slist_stud *head = malloc(sizeof(struct slist_stud));
    MEM_ALLOC_FAIL_RET(head, NULL);
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

void free_slist(struct slist_stud *head) {
    while (!SLIST_EMPTY(head))
    {
        struct student *s = SLIST_FIRST(head);
        SLIST_REMOVE_HEAD(head, classmates);
        free(s->name);
        free(s);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 2) return -1;

    struct slist_stud *snames = instantiate_slist(argv[1]);

    if (snames == NULL) _exit(EXIT_FAILURE);
    else if (SLIST_EMPTY(snames)) {
        free(snames);
        _exit(EXIT_FAILURE);
    }

    int i = 0;
    struct student *el;
    SLIST_FOREACH(el, snames, classmates)
    {
        printf("OUTPUT: i %d len %zu '%s' '%zu'\n", i++, el->nlen, el->name, el->hash);
    }

    free_slist(snames);
    free(snames);
    return 0;
}
