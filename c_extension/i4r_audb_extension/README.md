# I4R AUDB PostgreSQL Extension

A PostgreSQL extension implementing the I4R AUDB system in C.

## Overview

This is the primary implementation of the project. The extension exposes custom
types, operators, and functions through PostgreSQL's extension interface while
performing the core logic in native C.

## Building

```bash
make
make install
```

Refer to the project root README for complete setup instructions.

## Project Structure

This project intentionally follows the conventional layout used by many
PostgreSQL extensions. Source files, headers, SQL definitions, and the
`Makefile` are kept in a mostly flat directory because PostgreSQL's PGXS build
system is designed around this structure.

As the codebase grows, functionality is separated into multiple translation
units (for example, arithmetic, pruning, serialization, and logical operators)
rather than nested source directories.

## Directory Contents

- `*.c` — Extension implementation
- `*.h` — Internal headers
- `*.sql` — Extension installation and testing scripts
- `*.control` — PostgreSQL extension metadata
- `Makefile` — PGXS build configuration