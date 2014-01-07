# Auto generated configuration file
# using: 
# Revision: 1.194 
# Source: /cvs_server/repositories/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: reco -s RAW2DIGI,L1Reco,RECO,DQM --data --magField AutoFromDBCurrent --scenario pp --datatier RECO --eventcontent RECO,DQM --customise Configuration/GlobalRuns/reco_TLR_38X.py --cust_function customisePPData --no_exec --python_filename=rereco_Collision_38X.py --conditions auto:com --process RERECO
import FWCore.ParameterSet.Config as cms

process = cms.Process('SKIM')

# import of standard configurations

process.load('Configuration.EventContent.EventContent_cff')


process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    annotation = cms.untracked.string('reco nevts:1'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.options = cms.untracked.PSet(

)
# Input source


import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

if(sys.argv[2].find("/castor/")==0):
    filePrefex="rfio:"

process.source = cms.Source("PoolSource",
   #inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                          fileNames = cms.untracked.vstring(),
)

for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])

# Output definition
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
                                  #outputCommands = cms.untracked.vstring("keep *"),
    fileName = cms.untracked.string(sys.argv[len(sys.argv)-1]),
   
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('RECO'),
        filterName = cms.untracked.string('')
    )
)



#process.endjob_step = cms.Path(process.endOfProcess)
process.out_step = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.out_step)
 # /store/express/Run2011A/ExpressPhysics/FEVT/Express-v4/000/166/380/4E314DA8-1B8D-E011-AE53-001D09F25479.root

process.source.eventsToProcess = cms.untracked.VEventRange (
    "194424:409876904","194429:138566800","195378:1264451578"

    )

