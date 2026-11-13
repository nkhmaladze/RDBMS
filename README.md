# RDBMS

The storage and query-execution layers of a relational database, written in C++. I built each layer for a database systems course on top of SwatDB, the course's teaching database. They are collected here with their original commit history and tests.

## Status

**Work in progress.** These layers were written against SwatDB's library, which the course does not distribute, so they do not build from a clean clone yet. For now this repository is code to read, not a program to run.

The goal is one self-contained database with a single build: the missing disk, catalog and index layers underneath, and a SQL front end and shell on top.

## Layers

```
RelOpsManager     select, project, join             relops/  relops-join/
      │
  HeapFile        records in a file of pages        heap-file/
      │
  HeapPage        records in one slotted page       heap-page/
      │
BufferManager     pages cached in memory            buffer-manager/
      │
(disk manager)    part of SwatDB, not in this repository
```

| Directory | What it implements | Tests |
|---|---|---|
| [`buffer-manager/`](buffer-manager) | A buffer pool with pin counts and dirty-page tracking, a hash map from page to frame, and Clock and Random replacement policies behind one interface. | 35 |
| [`heap-page/`](heap-page) | A slotted page: a slot directory, record insert, get, update and delete, compaction on delete, and a page scanner. | 22 |
| [`heap-file/`](heap-file) | A file of heap pages with a header page, lists of free and full pages, record operations by record ID, and a file scanner. | 30 |
| [`relops/`](relops) | Select, by file scan or hash-index scan, and project. | 26 |
| [`relops-join/`](relops-join) | Tuple, block and index nested-loop joins. | 21 |

`relops-join/` extends `relops/` and carries its own copies of the shared manager and operation files.

## Tests

The 134 test cases use UnitTest++ and sit next to the code in each directory. Like the layers, they need SwatDB to build.

## History

Each layer started as its own repository. All five were imported here with their commits intact, so the full history of every layer is in this repository's log.

## Credits

- The buffer manager was written with Omar Ebied.
- The heap page was written with Zixi Gao.
- Select and project were written with Xezel Peshkalai.
- The class interfaces follow the outline provided by the course. SwatDB itself belongs to its authors and is not included here.
