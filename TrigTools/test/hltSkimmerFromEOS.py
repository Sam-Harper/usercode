

def getInputFiles(runEra,datasetName,runnr):
    runDir='/eos/cms/tier0/store/data/%s/%s/RAW/v1/000/%s/%s/' % (runEra,datasetName,runnr[0:3],runnr[3:6])
    print runDir
    import subprocess
     
    runSubDirs,err = subprocess.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',runDir],stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
    inputFiles=[]
    for subDir in runSubDirs.splitlines():
        fullDir = runDir.rstrip("/")+"/"+subDir
        out,err = subprocess.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',fullDir],stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
        for line in out.splitlines():
            inputFiles.append(fullDir.rstrip("/")+"/"+line)
    
    print inputFiles
    return inputFiles


import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HLTSkim")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
                        #    eventsToProcess = cms.untracked.VEventRange("1:1484800-1:1484810"),
#                            eventsToSkip = cms.untracked.VEventRange("1:1484806-1:1484806")
                             )


import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
options.register ('hltPaths',
                  "HLT_*",
                  VarParsing.VarParsing.multiplicity.list,
                  VarParsing.VarParsing.varType.string,          
                  "HLT paths to select")
options.register ('runnr',
                  "000000",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,          
                  "runnr")
options.register ('dataset',
                  "DoubleEG",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,          
                  "name of the dataset")
options.register ('runEra',
                  "Run2016B",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,          
                  "run era") 
options.setupTags (tag = '%s',
                ifCond = 'runnr',
                tagArg = 'runnr')
options.setupTags (tag = '%s',
                ifCond = 'dataset',
                tagArg = 'dataset')
options.parseArguments()


from SHarper.SHNtupliser.addInputFiles import addInputFiles
addInputFiles(process.source,
              getInputFiles(options.runEra,options.dataset,options.runnr))
    

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)




import HLTrigger.HLTfilters.hltHighLevel_cfi
process.skimHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.skimHLTFilter.HLTPaths = cms.vstring(options.hltPaths)

process.skimPath = cms.Path(process.skimHLTFilter)

process.load('Configuration/EventContent/EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
#  outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
 # outputCommands = process.AODSIMEventContent.outputCommands,
    outputCommands = cms.untracked.vstring("keep *"),
    fileName = cms.untracked.string(options.outputFile),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('skimPath')),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('HLTDEBUG'),                                     
    )
)
process.outPath = cms.EndPath(process.output)
