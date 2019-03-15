#!/usr/bin/env python

import argparse
import subprocess

if __name__ == "__main__":    
    parser = argparse.ArgumentParser(description='reads DQM histogram files and produces formated plots for easier validation')
    parser.add_argument('--scenario',help='scenario',required=True)
    parser.add_argument('--out_base','-o',help='base output name',required=True)
    args = parser.parse_args()

    input_files = "file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RECO/doubleElePt1To100_RECOWithSC_{scenario}_1.root,file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RECO/doubleElePt1To100_RECOWithSC_{scenario}_2.root,file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RECO/doubleElePt1To100_RECOWithSC_{scenario}_3.root,file:/mercury/data1/harper/mcFiles/pfRecHitValid/DoubleElePt1To100_RECO/doubleElePt1To100_RECOWithSC_{scenario}_4.root"
    

    output_file = args.out_base.format(scenario=args.scenario)
    #output_file = output_file.replace(".root","_{}.root".format(file_nr+1))
    input_files = input_files.format(scenario=args.scenario)
    print "doing file ",input_files
    cmd = ["cmsRun","SHarper/TrigNtup/test/scRegTreeMaker.py",
           "inputFiles={}".format(input_files),
           "outputFile={}".format(output_file)]
    print cmd
    out,err = subprocess.Popen(cmd).communicate()
    print out,err
        

        
