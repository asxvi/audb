# PL/pgSQL Reference Implementation

A PL/pgSQL implementation of the I4R AUDB system.

## Overview

This implementation provides the same functionality using PostgreSQL's native
procedural language instead of a compiled C extension.

It primarily exists as:

- quick prototyping and POC
- a reference implementation,
- a correctness baseline,
- and a comparison point for the C extension.

The C extension is the primary implementation and is intended for production
use and performance evaluation.