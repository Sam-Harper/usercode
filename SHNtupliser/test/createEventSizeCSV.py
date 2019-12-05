#!/usr/bin/env python

import argparse
import subprocess

def read_edmEventSize_output(raw_output,label,parsed_output={}):
    in_body = False
    for line in raw_output.split("\n"):
        if not in_body:
            if line.find("Branch Name |")==0:
                in_body = True
        else:
            if line=="": continue            
            name,uncomp,comp = line.split()
            if name not in parsed_output:
                parsed_output[name] = {}
            if label in parsed_output[name]:
                raise Exception("error label {} is repeated".format(label))

            parsed_output[name][label] = {'uncomp' : uncomp, 'comp' : comp}
    return parsed_output
        

def main():
    
    parser = argparse.ArgumentParser(description='converts edmEventSize to a csv, with a column for each file specified')
    parser.add_argument("--input_files",required=True,help="input files to run on",nargs="+")
    parser.add_argument("--labels",required=True,help="labels of input files",nargs="+")
    parser.add_argument('--out',required=True,help="output csv file")
    args = parser.parse_args()

    results = {}
    
    if len(args.input_files) != len(args.labels):
        raise Exception("input_files and labels are difference sizes {} vs {}".format(len(args.input_files),len(args.labels)))
    
    for filename,label in zip(args.input_files,args.labels):
        out,err = subprocess.Popen(["edmEventSize","-v",filename],stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
        if err!="":
            print "errors occured\n{}".format(err)
        else:
            read_edmEventSize_output(out,label,results)
           

    out_strs = []

    header_str = "product,"+",".join(["{}"]*len(args.labels)).format(*args.labels)
    line_str = "{name},"+",".join(["{{v[{}][comp]}}"]*len(args.labels)).format(*args.labels)
    out_strs.append(header_str)
    for key,values in results.iteritems():
        print key,values
        for label in args.labels:
            if label not in values:
                values[label]={'comp' : 0., 'uncomp' : 0.}
                
        out_strs.append(line_str.format(name=key,v=values))
    with open(args.out,'w') as f:
        f.write('\n'.join(out_strs))

if __name__ == "__main__":
    main()
