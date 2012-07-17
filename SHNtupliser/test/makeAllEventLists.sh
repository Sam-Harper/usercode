#!/usr/bin/env bash

eventList =$1
nrJobs=$2

for jobNr in seq `1 $nrJobs`
do
echo ./makeEventList.sh temp $jobNr $eventList
done
