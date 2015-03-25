import FWCore.ParameterSet.Config as cms

def addInputFiles(source,files):

    #now check if its dataset name or a text file containing the files (not implimented yet)
    if(files[0].find(".root")==-1):
        from SHarper.SHNtupliser.dasFileQuery import dasFileQuery
        files=dasFileQuery(files)

    
    filePrefex="file:"
    if(files[0].find("/pnfs/")==0):
        filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"
    if(files[0].find("/store/")==0):
        filePrefex=""
    if(files[0].find("/eos/")==0):
        filePrefex="'root://eoscms/"

 
    for i in range(0,len(files)):
        print filePrefex+files[i]
        source.fileNames.extend([filePrefex+files[i],])
    
        
    
    
    
