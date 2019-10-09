
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

#setup global tag, note it is incredably important to due use the correct global tag
#which is the tag the MC or data was recoed with
#if not, you'll load in the wrong menu and get the wrong bit names and prescales 
#a MC tag for data will give something that looks like it works but actually doesnt 
#and I persum vice versa.
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
from Configuration.AlCa.GlobalTag import GlobalTag
if options.isMC:
    process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_realistic_v15', '')
 #   process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v14','')
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

