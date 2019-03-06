
# Import configurations
import FWCore.ParameterSet.Config as cms
# set up process
process = cms.Process("L1Example")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.register('isMC',True,options.multiplicity.singleton,options.varType.bool," whether we are running on MC or not")
options.parseArguments()

print options.inputFiles
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
                          )



# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
    limit = cms.untracked.int32(10000000)
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
from Configuration.AlCa.GlobalTag import GlobalTag
if options.isMC:
    process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_realistic_v15', '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, '102X_dataRun2_Prompt_v11','')


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.l1MenuExample = cms.EDAnalyzer("L1MenuExample",
                                       hltProcess=cms.string("HLT")
                                       )

process.p = cms.Path(process.l1MenuExample)

