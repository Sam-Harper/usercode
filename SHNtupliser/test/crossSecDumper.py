# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("USER")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)




import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)

if sys.argv[2].find(".root")==-1:
    print "doing dataset file look up ",sys.argv[2]
    from SHarper.SHNtupliser.dasFileQuery import dasFileQuery
    files = dasFileQuery(sys.argv[2])
    for filename in files:
        print filename
        process.source.fileNames.extend([str(filename),])

    
    
else:
    for i in range(2,len(sys.argv)):
        print filePrefex+sys.argv[i]
        process.source.fileNames.extend([filePrefex+sys.argv[i],])
    
process.crossSecDumper = cms.EDAnalyzer("CrossSecDumper",
                                  
                                          )




process.p = cms.Path(process.crossSecDumper)

