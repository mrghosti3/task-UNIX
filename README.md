# UNIX task

This is a Vilnius University (VU) Faculty of Mathematics and Informatics (MIF)
Informatics Technologies UNIX Operating Systems course task.

## Task

- Fetch and extract FreeBSD source:
    ftp://ftp.freebsd.org/pub/FreeBSD/releases/amd64/13.0-RELEASE/src.txz

- Use the tree and the queue implementation from FreeBSD kernel:

```
     sys/sys/tree.h
     sys/sys/queue.h
```

- Use hash implementation from xxhash: https://github.com/Cyan4973/xxHash

>  Note: you don't need to compile the project to use it.
>        it provices ".h only" implementation which means
>        you just need to download one single header file.

### The Code

1. take the full names of your colleagues in your group
    and make an array of pointers.
2. insert this array into a binary tree it's key being
    a full name's hash and value a pointer to a full name.
3. implement and use the following functionality to
    manipulate your tree besides inserting into it:

```
    search
    delete
    traverse (full tree)
```

4. the code must deal with the collisions.
    - one (1) point is given for the correctly implemented
        project which correctly recognizes the hash
        collision case but does handle it by just
        issuing a warning on s duplicate key for a
        different value.
    - two (2) points are given for the correctly implemented
        project which correctly recognizes the hash
        collision case and does handle it by implementing
        a chain per tree node in a form of an appropriate
        linked list.

## Author

- [Robertas Timukas](https://git.mif.vu.lt/roti7541) (alias: [mrghosti3](https://github.com/mrghosti3)) robertas.timukas@mif.stud.vu.lt
