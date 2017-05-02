
#!/usr/bin/env python

import os
import argparse

parser = argparse.ArgumentParser(description='analysers all the igreports')
parser.add_argument('igReports',help='igReports',nargs="+")
parser.add_argument('--memOpt',help='MEM profile option',default="MEM_LIVE")
args = parser.parse_args()

for report in args.igReports:
    
    outputFilename = report.replace(".gz",".txt")
    import subprocess
    output= subprocess.Popen(['igprof-analyse',"-d","-v","-g","-r","MEM_LIVE",report],stdout=subprocess.PIPE).communicate()[0].splitlines()

    outputFile=open(outputFilename,"w")
    for line in output:
        outputFile.write(line+"\n")
    outputFile.close()
