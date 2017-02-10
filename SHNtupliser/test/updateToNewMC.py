#!/usr/bin/env python

input_file="SHNtupliser/test/2016Datasets80XMiniAOD"

newmc_file="miniAODDatasets"

newmc={}

with open(newmc_file) as f:
    for line in f.readlines():
        newmc[line.split("/")[1]]=line.rstrip()

with open(input_file) as f:
    for line in f.readlines():
        try:
            mcname = line.split("&")[0].split("/")[1]
            if mcname in newmc:
                print line.replace( line.split("&")[0],newmc[mcname]).rstrip()
            else:
                print line.rstrip()
        except IndexError:
            print line
