

#this config file configures the PAT to be suitable for HEEP analyses and then runs an example analyzer
#this uses only bare pat and does not use the HEEP event

isMC=True
# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# Load geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
if isMC:
    process.GlobalTag.globaltag = autoCond['startup'] 
else:
    process.GlobalTag.globaltag = autoCond['com10']

process.load("Configuration.StandardSequences.MagneticField_cff")


# this defines the input files
import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-1):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])




# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.heepPatElectrons = cms.EDProducer("HEEPAttStatusToPAT",
                                          eleLabel = cms.InputTag("patElectrons"),
                                          barrelCuts = cms.PSet(heepBarrelCuts),
                                          endcapCuts = cms.PSet(heepEndcapCuts),
                                          applyRhoCorrToEleIsol = cms.bool(False), 
                                          eleIsolEffectiveAreas = cms.PSet (
                                              trackerBarrel = cms.double(0.),
                                              trackerEndcap = cms.double(0.),
                                              ecalBarrel = cms.double(0.101),
                                              ecalEndcap = cms.double(0.046),
                                              hcalBarrel = cms.double(0.021),
                                              hcalEndcap = cms.double(0.040)
                                              ),
                                          eleRhoCorrLabel = cms.InputTag("kt6PFJets","rho"),
                                          )

process.heepAnalyzerHEEPPAT = cms.EDAnalyzer("HEEPAnalyzerHEEPPAT",
                                             eleLabel = cms.InputTag("heepPatElectrons"),
                                             )


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)
#this is a safety to stop the unwary deleteing their input file ;)
if len(sys.argv)>2:
    process.TFileService.fileName = cms.string(sys.argv[len(sys.argv)-1])

#this is only here because you need an output module named out to remove the taus from pat
#the error is very helpful though:
###An exception of category 'ConfigFileReadError' occurred while
###   [0] Processing the python configuration file named HEEPAnalyzer/test/HEEPAnalyzerHEEPPAT_cfg.py
###Exception Message:
###python encountered the error: <type 'exceptions.TypeError'>
###raise: arg 3 must be a traceback or None
#sigh
process.load("Configuration.EventContent.EventContent_cff")
process.out = cms.OutputModule("PoolOutputModule",
    process.FEVTEventContent,
    dataset = cms.untracked.PSet(dataTier = cms.untracked.string('RECO')),
     fileName = cms.untracked.string("eh.root"),
)


#no configuration of the pat is necessary for us at the moment
process.load("PhysicsTools.PatAlgos.patSequences_cff")

### Taus are currently broken in 523, luckly I copied this example from a 41X config file where taus were also broken
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process, ['Taus'] )
process.patDefaultSequence.remove( process.patTaus )

##define the heep energy corrector
process.load("SHarper.HEEPAnalyzer.gsfElectronsHEEPCorrs_cfi")
## we also need to re-run the electron ID value maps as they will be no longer valid
process.load("RecoEgamma.ElectronIdentification.electronIdSequence_cff")


process.p = cms.Path(process.gsfElectronsHEEPCorr*process.eIdSequence* #make the new energy corrected gsf electrons and id value maps
                     process.patDefaultSequence* #runs PAT 
                     process.heepPatElectrons* #heepifies the pat electrons (resets energy to ecal energy and adds heep id)
                     process.heepAnalyzerHEEPPAT) #runs heep analyzer

#now we need to swap everything over to the new gsfElectron collection, this should be last in the config
from SHarper.HEEPAnalyzer.heepTools import *
swapCollection(process,"gsfElectrons","gsfElectronsHEEPCorr")
