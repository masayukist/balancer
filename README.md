----------------------------------------------
Balancer: Command-level parallelization by MPI
----------------------------------------------

How to use
==========

### Terminology

A front-end of a command (FEC) : A part of a command line, which does
not change among parallelized command lines.

A back-end of a command (BEC) : A part of a command line, which
changes among parallelized command lines.

### Command Line Usage

> make
> mkdir results
> vi FEC_line_file or emacs FEC_line_file   # preparing a file in which FEC is described by single line.
> vi BEC_list_file or emacs BEC_list_file   # preparing a file listing BEC
> ./balancer FEC_line_file BEC_list_file [BEC_list_file2 BEC_list_file3 ...]


File description
================

### balancer.cpp

MPI application written in C++

### makefile

THe file contains required commands for building balancer.cpp and
generating balancer binary.
