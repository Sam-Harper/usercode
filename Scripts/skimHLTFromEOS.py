#!/usr/bin/env python
      
from   subprocess import Popen , PIPE
       
## check aliases/functions,
def _check_ ( command ) :
 
    try :
        p   = Popen( command , stderr = PIPE , stdout = PIPE , shell = True )
        out,err = p.communicate()
        for l in err.splitlines() : return False
        return 0 == p.returncode  
    except OSError :
        return False
    
def _alias_ ( command ) :
    
    p   = Popen( [ '-i' , '-c' , 'alias -p'],
                 stderr = PIPE , stdout = PIPE , executable = '/bin/bash' )
    out,err = p.communicate()
    for l in err.splitlines() : return False
    if 0 != p.returncode      : return False
    cmd = 'alias %s=' % command.strip()
    for line in out.splitlines() :
        p = line.find( cmd )
        if 0 != p : continue
        return line[len(cmd):-1].strip('"').strip("'")
    return False
                       
        
def _getcmd_ ( command )      :
    ## try to check it as command
    if _check_    ( command ) : return command
    ## try to check as alias
    cmd = _alias_ ( command )
    if cmd                    : return cmd
    ## try to read the configuration file
    cmd = _read_setup_ ( command , setup + '.sh' )
    if not cmd : raise OSError ( "Can't get correct command for '%s'" % command )
    return cmd
 

 

import os
import argparse

parser = argparse.ArgumentParser(description='makes a list of files to run over')

parser.add_argument('dir',help='starting directory',type=str)
parser.add_argument('--config',help="cmssw config file",required=True)
parser.add_argument('--hltMatch',help="hltMatching critera",required=True)


parser.add_argument('-d', '--fullPath', help="lists the full path",
                    action="store_true")
parser.add_argument('-n', '--singleLine', help="puts it in a single line",
                    action="store_true")

args = parser.parse_args()
import subprocess
out,err = subprocess.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',args.dir],stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()


output=""

for line in out.splitlines():
    if args.fullPath:
        line=args.dir+"/"+line
    output+=line
    if args.singleLine:
        output+=" "
    else:
        output+="\n "

print output
#subDirs = subprocess.Popen(['eos','ls',args.dir],stdout=subprocess.PIPE).communicate()[0]

#for subDir in subDirs:
 #   print subDir
#with EOS() as eos:
#    print "test"
