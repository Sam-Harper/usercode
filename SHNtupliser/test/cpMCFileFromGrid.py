
import argparse
import subprocess
import time
import sys

parser = argparse.ArgumentParser(description='gives the files and optional picks the events given')
parser.add_argument('filename',type=str,help='filename')
parser.add_argument('--output_dir','-o',default="./",help='output directory')
args = parser.parse_args()

if args.filename.find("/store/mc/")!=0:
    print "filename {} is not a valid MC path".format(args.filename)
    sys.exit()

file_data = {'dataset' : args.filename.split('/')[4],
             'campaign' : args.filename.split('/')[3],
             'conditions' : args.filename.split('/')[6],
             'datatier' : args.filename.split('/')[5],
             'filename' : args.filename.split('/')[-1]
             }
 
print file_data            

outfilename = "{dataset}__{campaign}__{conditions}__{datatier}__{filename}".format(**file_data)
cp_args="xrdcp root://xrootd-cms.infn.it//{} {}/{}".format(args.filename,args.output_dir,outfilename)
print ""
print "out filename:"
print "    {}/{}".format(args.output_dir,outfilename)

print ""
print cp_args
time.sleep(5)
subprocess.Popen(cp_args.split()).communicate()

