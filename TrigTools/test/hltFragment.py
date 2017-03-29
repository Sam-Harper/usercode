#>>>From hltFragment <<<#
from SHarper.TrigTools.hltConfigTools import *
process.maxEvents.input=-1
rmAllEndPathsWithOutput(process)
addOutputMod(process)

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
options.parseArguments()

process.hltOutputTot.fileName=cms.untracked.string(options.outputFile)
process.MessageLogger.suppressWarning.extend(["hltEgammaGsfTracks","hltEgammaGsfTracksUnseeded"])
print  "global tag : ",process.GlobalTag.globaltag


#delattr(process,"EvFDaqDirector")
#delattr(process,"FastMonitoringService")
#delattr(process,"DQMStore")
#delattr(process,"hltDQMFileSaver")
#delattr(process,"RatesMonitoring")
#delattr(process,"MessageLogger")
