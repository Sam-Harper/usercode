#!/usr/env python

import os
import sys
import shutil
import argparse
import glob
import datetime
import subprocess

def make_executable(path):
    mode = os.stat(path).st_mode
    mode |= (mode & 0444) >> 2    # copy R bits to X
    os.chmod(path, mode)

def buildTar_(tgzNameWithPath):
    swArea=os.environ['CMSSW_BASE']
    import tarfile
    tar = tarfile.open(tgzNameWithPath, "w:gz")
    lib = swArea+'/' +"lib"
    if os.path.exists(lib):
        print "adding ",lib
        tar.add(lib)
        print "added ",lib

    for package in os.listdir(swArea+"/src"):
        if os.path.isdir(swArea+"/src/"+package):
            for subPackage in os.listdir(swArea+"/src/"+package):
                if os.path.isdir(swArea+"/src/"+package+"/"+subPackage):
                     for subDir in os.listdir(swArea+"/src/"+package+"/"+subPackage):
                         if subDir=='data':
                             tar.add("src/"+package+"/"+subPackage+"/data");
                             print "adding dataDir: " + swArea+"/src/"+package+"/"+subPackage+"/data"
 

 
       

def create_release_area(cfg):
    #right so there is a wee bit of a feature in scram, it can only make directorys
    #as sub directories of the current dir
    #so we go to / and then come back
    cwd = os.getcwd()
    os.chdir('/')
    scram_cmd = 'scram proj -n {working_area} CMSSW {cmssw_version}'.format(**cfg).split()
    subprocess.Popen(scram_cmd).communicate()
    os.chdir(cwd)
    shutil.copy(cfg['in_proxy_file'],cfg['out_proxy_file'])
    dirs_to_copy = ['lib','python','cfipython']
    for dir_to_copy in dirs_to_copy:
        source = "{}/{}".format(cfg['orginal_area'],dir_to_copy)
        target = "{}/{}".format(cfg['working_area'],dir_to_copy)
        if os.path.exists(target):
            shutil.rmtree(target)
            shutil.copytree(source,target)
        

    org_src_dir = '{}/src'.format(cfg['orginal_area'])
    for package in os.listdir(org_src_dir):
        package_dir = '{}/{}'.format(org_src_dir,package)
        if os.path.isdir(package_dir):
            for subpackage in os.listdir(package_dir):
                subpackage_dir = '{}/{}'.format(package_dir,subpackage) 
                if os.path.isdir(subpackage_dir):
                    for subdir in os.listdir(subpackage_dir):
                        if subdir=='data':
                            target_dir = '{cfg[working_area]}/src/{package}/{subpackage}'.format(cfg=cfg,package=package,subpackage=subpackage)                             
                            os.makedirs(target_dir)
                            shutil.copytree("{}/data".format(subpackage_dir),"{}/data".format(target_dir))
                            print "adding dataDir: {}/data".format(target_dir)
                             

def split_input(input_files_raw,nrjobs,file_prefix=""):

    if ".list" in input_files_raw:
        input_files = [line.strip().replace("/pnfs/pp.rl.ac.uk/data/cms","").replace("/opt/","file:/opt/") for line in open(input_files_raw)]

    else:
        #try and pattern match, if it matches nothing, use the orginal input_files
        input_files=glob.glob(input_files_raw)
        if not input_files:
            input_files = [input_files_raw]
    input_files = [file_prefix + x for x in input_files]
    print input_files
    input_files_job=nrjobs*[None];

    nrfiles=len(input_files)
    nrfiles_job=nrfiles/nrjobs
    nrfiles_left=nrfiles%nrjobs
    
    for jobnr in range(0,nrjobs):
        min_filenr=jobnr*nrfiles_job
        max_filenr=(jobnr+1)*nrfiles_job
        input_files_job[jobnr]=input_files[min_filenr:max_filenr]

    for leftnr in range(0,nrfiles_left):
        input_files_job[leftnr].append(input_files[(jobnr+1)*nrfiles_job+leftnr])
                                 
    return input_files_job

def make_queue_cmd(cfg,args):
    queue_cmd = "Queue Arguments From (\n"
    
    for jobnr in range(0,cfg['nrjobs']):
        cmsrun_args = "cmssw.py"
        
        if args.globalTag!=None:
            cmsrun_args+=" globalTag="+args.globalTag
        for filename in cfg['input_files_jobs'][jobnr]:
            if args.interfaceType==1 or args.interfaceType==2:
                if cmsrun_args.find("inputFiles=")==-1: 
                    cmsrun_args+=" inputFiles="
                else: 
                    cmsrun_args+=","
            else: cmsrun_args+=" "
            cmsrun_args+=filename

            cmsrun_args+=" "
            if args.interfaceType==1: cmsrun_args+="outputFile="
            if args.interfaceType==2: cmsrun_args+="outFile="
            cmsrun_args+="$TMPDIR/{}\n".format(cfg['out_file'].format(jobnr=jobnr))
            
            queue_cmd+=cmsrun_args
    queue_cmd += ")\n"
    return queue_cmd

def make_cfg(args):
    cfg = {}
    cfg['job_script'] = 'runCMSSW.sh'
    cfg['nrjobs'] = args.nrJobs
    cfg['cfg_name'] = args.config

    cfg['base_dir']="/opt/ppd/month/harper" 
    cfg['base_outdir'] = "{}/{}".format(cfg['base_dir'],args.baseOutDir)
    cfg['base_batchdir'] = "{}/{}".format(cfg['base_dir'],"cmsswBatchJobFiles")
    cfg['cmssw_version'] = os.environ['CMSSW_VERSION']
    cfg['orginal_area'] = os.environ['CMSSW_BASE']
    cfg['working_area'] = "{cfg[base_batchdir]}/{cfg[cmssw_version]}.{time}".format(cfg=cfg,time=datetime.datetime.now().strftime('%Y%m%d_%H%M%S'))
    cfg['out_dir'] = '{}/{}/{}'.format(cfg['base_outdir'],cfg['cmssw_version'][6:],args.outputDir)
    cfg['log_dir'] = '{out_dir}/logs'.format(**cfg)
    cfg['in_proxy_file'] = '/tmp/x509up_u27618'
    cfg['out_proxy_file'] = '{}/src/grid_proxy'.format(cfg['working_area'])
    
    if args.output!=None:
        cfg['out_file'] = "{}_{{jobnr}}.{}".format(args.output.rsplit('.',1)[0],args.output.rsplit('.',1)[1])
        cfg['out_file_base'] = args.output.rsplit('.',1)[0]
    else:
        cfg['out_file'] = "{}_{{jobnr}}.root".format(args.outputDir.split("/")[-1])
        cfg['out_file_base'] = args.outputDir.split("/")[-1]
        
    cfg['input_files_jobs'] = split_input(args.input,cfg['nrjobs'],args.filePrefix)
    cfg['queue_cmd'] = make_queue_cmd(cfg,args)
    return cfg
     
def make_job_script_txt(cfg):
    txt = """
#!/usr/bin/env bash
source ~/.bash_profile
echo $TMPDIR
cd {working_area}/src
cmsenv
eval `scram runtime -sh`
echo $CMSSW_RELEASE_BASE $CMSSW_BASE
export X509_USER_PROXY={out_proxy_file}
echo $X509_USER_PROXY
voms-proxy-info -all
echo cmsRun $*
cmsRun $*
echo "working area contents:"
ls -lh
echo "tmp dir contents:"
ls $TMPDIR -lh
cp $TMPDIR/{out_file_base}* {out_dir}
""".format(**cfg)
    return txt

def make_submit_txt(cfg):
    txt ="""
Universe               = vanilla
Executable             = {working_area}/src/{job_script}
Log                    = {log_dir}/job_$(Process).log
Output                 = {log_dir}/job_$(Process)_out.log
Error                  = {log_dir}/job_$(Process)_err.log
Request_memory         = 8 GB
Getenv                 = False  
initialdir             = {working_area}/src
requirements           = (OpSysAndVer =?= "CentOS7")
request_cpus           = 4

{queue_cmd}
""".format(**cfg)
    return txt

def main():
    
    parser = argparse.ArgumentParser(description='submits CMSSW jobs to RAL batch system')
    parser.add_argument('--config',help='cmsRun config file to run',required=True)
    parser.add_argument('--input',help='input file or file pattern to run over',required=True)
    parser.add_argument('--output',help='output filebase name, defaults to outputDir+.root',default=None)
    parser.add_argument('--outputDir',help='ouput dir (under scratch/mc/CMSSWVersion/<outputdir>',required=True)
    parser.add_argument('--interfaceType',help='interface type of python config file (0=SamStd, 1=varparsing std,=2 varparsing extended',default=0,type=int)
    parser.add_argument('--runLocal',help='run locally rather than batch',action='store_true')
    parser.add_argument('--nrJobs',help='number of jobs to split into (can not be larger than #files to run over)',default=1,type=int)
    parser.add_argument('--baseOutDir',help='base output directory',default="mc")
    parser.add_argument('--globalTag',help='globalTag',default=None)
    parser.add_argument('--filePrefix',help='prefix to add to filenames',default="")

    args = parser.parse_args()
    print args.config

    cfg = make_cfg(args)
    
    if os.path.exists(cfg['out_dir']):
        print "output directory {out_dir} exists, aborting ".format(**cfg)
        exit(1)
    os.makedirs(cfg['out_dir'])
    if not os.path.exists(cfg['log_dir']):
        os.makedirs(cfg['log_dir'])
        
    create_release_area(cfg)
    shutil.copyfile(args.config,"{working_area}/src/cmssw.py".format(**cfg))
        
    job_script ="{working_area}/src/{job_script}".format(**cfg)
    condor_subfile = "{working_area}/src/condorJobFile".format(**cfg)
    
    with open(condor_subfile,"w") as f:
        f.write(make_submit_txt(cfg))

    with open(job_script,"w") as f:
        f.write(make_job_script_txt(cfg))
    make_executable(job_script)

    print   condor_subfile

    print "condor_submit {}".format(condor_subfile)
    subprocess.Popen(['condor_submit',condor_subfile]).communicate()

if __name__ == "__main__":
    main()
