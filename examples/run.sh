#!/bin/bash

#Example on AOBA system

#PBS -T openmpi
#PBS -N balancer_test
#PBS -l elapstim_req=1:00:00
#PBS -q lx
#PBS -b 2
#PBS -o stdout.txt
#PBS -e stderr.txt

N_PROCESS=256

if [ ! -z "${PBS_O_WORKDIR}" ]
then
    cd ${PBS_O_WORKDIR}
fi

mpirun ${NQSV_MPIOPTS} -np ${N_PROCESS} ../balancer ./command.txt ./arguments.txt
