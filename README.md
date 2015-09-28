--------------------------------------------
Balancer: Parallelizing command lines by MPI
--------------------------------------------

How to
======

### Terminology

command line: what you want to execute in a command-line interpreter
(bash, zsh, etc...)

command: A part of a command line, which does not change among
parallelized command lines.

arguments: A part of a command line, which changes among parallelized
command lines.

### Usage

Ex. when you want to execute following command lines in parallel fashion.
$ prog args1 > args1-out.txt
$ prog args2 > args2-out.txt
$ prog args3 > args3-out.txt
$ prog args4 > args4-out.txt

- Build Balancer
$ make

- Prepare directory for Balancer's output (optional)
$ mkdir results

- Prepare a file that contains the command, named ``command.txt``.
$ cat command.txt
hoge

- Prepare a file that contains the list of arguments, named ``arguments.txt``.
$ cat arguments.txt
args1 > args1-out.txt
args2 > args2-out.txt
args3 > args3-out.txt
args4 > args4-out.txt

- Execute Balancer in ``n`` parallel. (Please follow the manner of ``mpirun``.)
> mpirun -np ``n`` ./balancer command.txt arguments.txt

### Advanced

- Multiple files can be specified for the list of arguments
> ./balancer command.txt arguments.txt arugments_additional.txt

File description
================

### balancer.cpp

MPI application written in C++

### makefile

The file contains required commands for building balancer.cpp and
generating balancer binary.
