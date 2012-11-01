#this config places the HEEPId into allLayer1Electrons
isMC=False
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
process.load("Configuration.Geometry.GeometryIdeal_cff")
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

#setting up the producer to make the HEEP ID value map
from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.HEEPId = cms.EDProducer("HEEPIdValueMapProducer",
                                eleLabel = cms.InputTag("gsfElectrons"),
                                barrelCuts = cms.PSet(heepBarrelCuts),
                                endcapCuts = cms.PSet(heepEndcapCuts),
                                eleIsolEffectiveAreas = cms.PSet(heepEffectiveAreas),
                                eleRhoCorrLabel = cms.InputTag("kt6PFJetsForIsolation","rho"),
                                applyRhoCorrToEleIsol = cms.bool(True),
                                verticesLabel = cms.InputTag("offlinePrimaryVerticesWithBS"),
                                writeIdAsInt =cms.bool(True) #true saves the heep ID as an int, false: saves as a float, user request
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


#configure the pat to load the id in
process.load("PhysicsTools.PatAlgos.patSequences_cff");
### Taus are currently broken in 523, luckly I copied this example from a 41X config file where taus were also broken
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process, ['Taus'] )
process.patDefaultSequence.remove( process.patTaus )

if isMC==False:
    removeMCMatching(process, ['All'])


process.patElectrons.userData.userInts.src = cms.VInputTag('HEEPId')

#for isolation correction
from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
process.kt6PFJetsForIsolation = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True )
process.kt6PFJetsForIsolation.Rho_EtaMax = cms.double(2.5)


#little example analyzer
process.heepAnalyzer = cms.EDAnalyzer("HEEPAnalyzerPATHEEPId",
                                      eleLabel=cms.InputTag("patElectrons")
)

process.p = cms.Path(process.kt6PFJetsForIsolation*
                     process.HEEPId* #makes the HEEPID value map
                     process.patDefaultSequence* #runs pat
                     process.heepAnalyzer #example analyser module
                     ) 
