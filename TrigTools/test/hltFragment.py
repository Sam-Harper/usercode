#>>>From hltFragment <<<#
from SHarper.TrigTools.hltConfigTools import *
process.maxEvents.input=-1
rmAllEndPathsWithOutput(process)
rmPaths(process,["HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v3","HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v3"])
setSaveTags(process,"HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v3",True)
setSaveTags(process,"HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v3",True)
addOutputMod(process)

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
options.parseArguments()

isCrabJob=False #script seds this if its a crab job
#if 1, its a crab job...
if isCrabJob:
    process.hltOutputTot.fileName=cms.untracked.string("OUTPUTFILE")
else:
    process.hltOutputTot.fileName=cms.untracked.string(options.outputFile)



delattr(process,"EvFDaqDirector")
delattr(process,"FastMonitoringService")
delattr(process,"DQMStore")
delattr(process,"hltDQMFileSaver")
delattr(process,"RatesMonitoring")
delattr(process,"MessageLogger")
