#!/usr/bin/env python

import argparse
import subprocess

if __name__ == "__main__":    
    parser = argparse.ArgumentParser(description='reads DQM histogram files and produces formated plots for easier validation')
    parser.add_argument('--scenario',help='scenario',required=True)
    parser.add_argument('--out_base','-o',help='base output name',required=True)
    args = parser.parse_args()

    input_files = ["file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RAW/doubleElePt1To100_RAW_1.root","file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RAW/doubleElePt1To100_RAW_2.root","file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RAW/doubleElePt1To100_RAW_3.root","file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RAW/doubleElePt1To100_RAW_4.root"]

    for file_nr,input_file in enumerate(input_files):
        output_file = args.out_base.format(scenario=args.scenario)
        output_file = output_file.replace(".root","_{}.root".format(file_nr+1))
        print "doing file ",input_file
        cmd = ["cmsRun","makeReco_2017.py",
               "inputFiles={}".format(input_file),
               "outputFile={}".format(output_file),
               "pfRecHitThres={}".format(args.scenario)]
        print cmd
        out,err = subprocess.Popen(cmd).communicate()
        print out,err
        

        
