import FWCore.ParameterSet.Config as cms

def addInputFiles(source,files):

    #now check if its dataset name or a text file containing the files (not implimented yet)
    print(len(files))
    if(files[0].find(".root")==-1):
        import os
        if os.path.exists(files[0]):
            with open(files[0]) as f:
                content=f.readlines()
                files=[]
                for filename in content:
                    print(filename)
                    filename = filename.split("#")[0]
                    filename=filename.lstrip().rstrip()
                    files.append(filename)
        else:
            from SHarper.SHNtupliser.dasFileQuery import dasFileQuery
            files=dasFileQuery(files,-1)

        

    
    filePrefex="file:"
    if(files[0].find("/pnfs/")==0):
        filePrefex="root://dcap.pp.rl.ac.uk:1094"
    if(files[0].find("/store/")==0):
        filePrefex=""
    if(files[0].find("/eos/")==0):
        filePrefex="root://eoscms/"

 
    for i in range(0,len(files)):
        print(filePrefex+files[i].rstrip())
        source.fileNames.extend([filePrefex+files[i].rstrip(),])
    
        
    
    
    
