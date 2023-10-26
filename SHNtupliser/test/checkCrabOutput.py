import CRABClient
from CRABAPI.RawCommand import crabCommand
import tarfile
import datetime
import glob
import time

import contextlib
import sys
import os

class DummyFile(object):
    def write(self, x): pass

@contextlib.contextmanager
def nostdout():
    save_stdout = sys.stdout
    sys.stdout = DummyFile()
    yield
    sys.stdout = save_stdout

def find_missing_jobs(output_dir,nrjobs):
    file_extension = ".root"
    root_files = glob.glob(output_dir+"/0000/*"+file_extension)
    
    jobs_found=[]
    for root_file in root_files:
        file_name = root_file.split("/")[-1]
        jobnr = file_name.split("_")[-1].split(file_extension)[0]
        jobs_found.append(int(jobnr))
    
    jobs_notfound=[]
    for nr in range(1,nrjobs+1):
        if nr not in jobs_found:
            jobs_notfound.append(nr)
    return jobs_notfound
            

#there may be a small timestamp descripancy of a second or two
#so we need to figure it out
#we know its the format YYMMDD_HHMMSS
#so we compare YYMMDD_HHMM 
#with more knowledge, this can be 30 seconds out
def get_grid_output_dir(base_dir,timestamp):
    subdirs = [p.split("/")[-2] for p in glob.glob(base_dir+"/*/")]
#    print timestamp
    for subdir in subdirs:
#        help(datetime.datetime.strptime(subdir,"%y%m%d_%H%M%S"))
#        print datetime.datetime.strptime(subdir,"%y%m%d_%H%M%S").time().time()
        if timestamp[:-2]==subdir[:-2] and abs(int(timestamp[-2:])-int(subdir[-2:])<=30):
            return base_dir+"/"+subdir
    return base_dir+"/"+timestamp

def check_crab_output(crab_dir,resubmit_failed,verbose):
    
    #first get the config file to figure out where the data was written too
    try:
        job_data =tarfile.open(crab_dir+"/inputs/debugFiles.tgz")
    except FileNotFoundError as e:
        print(f"error getting config file\n{e}")
        return
    config_file = job_data.extractfile(job_data.getmember("debug/crabConfig.py"))
    config_python = config_file.read()
    config_dict = {"config" : None}
    exec(config_python,config_dict)
    config = config_dict["config"]

    with nostdout():
        try_nr = 1
        status = []
        while try_nr <=3 and status==[]:
            try:
                status = crabCommand('status',dir = crab_dir)
            except:
                try_nr += 1
                time.sleep(10)
   # print status
    if status == []:
        print(crab_dir," : failed to get crab status")
        return

    #jobsPerStatus
    nrjobs = len(status['jobList'])
    
    #right time to build the output dir
    timestamp = datetime.datetime.fromtimestamp(int(status['submissionTime'])).strftime('%y%m%d_%H%M%S')
    grid_output_dir = get_grid_output_dir("/pnfs/pp.rl.ac.uk/data/cms"+config.Data.outLFNDirBase+"/"+config.Data.inputDataset.split("/")[1]+"/"+config.Data.outputDatasetTag+"/",timestamp)
#    print grid_output_dir
    missing_jobs = find_missing_jobs(grid_output_dir,nrjobs)
    
    if status["taskWarningMsg"]!=[] and status["taskWarningMsg"]!='[]':
        print(crab_dir, ":",status["taskWarningMsg"][0])
    elif status['jobsPerStatus']=={}:
        print(crab_dir, ": no jobs created")
    elif not missing_jobs:
        print(crab_dir," : all jobs completed")
    else:
        print(crab_dir," : jobs missing")
        job_states={'failed' : [],'running' : [],'finished' : [],'idle' : [],'transferring' : []}
        
        for jobnr in missing_jobs:
            job_states[ status['jobs'][str(jobnr)]['State'] ].append(jobnr)
            #print "   ",jobnr,status['jobs'][str(jobnr)]['State']
        for state in job_states.keys():
            print(state,":",job_states[state])

        if resubmit_failed and job_states['failed']:
            print("resubmiting jobs",job_states['failed'] )
            crabCommand('resubmit',dir = crab_dir)

        if verbose:
            for jobnr in job_states['failed']:
                print(status['jobs'][str(jobnr)])

       # print crabCommand('report',dir = crab_dir)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='checks if the crab output is ')
    parser.add_argument('crab_dirs',nargs='+',help='list of crab directories to check')
    parser.add_argument('--resub_failed',action="store_true",help='resubmits failed jobs')
    parser.add_argument('--resub_finished',action="store_true",help='resubmits finished jobs')

    parser.add_argument('--verbose','-v',action="store_true",help='outputs a lot of infp')
    
    args = parser.parse_args()

    for crab_dir in args.crab_dirs:
        time.sleep(10)
        check_crab_output(crab_dir=crab_dir,resubmit_failed=args.resub_failed,verbose=args.verbose)



