import FWCore.ParameterSet.Config as cms

process = cms.Process("TestID")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(10000000)
)

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
# NOTE: the pick the right global tag!
#    for PHYS14 scenario PU4bx50 : global tag is ???
#    for PHYS14 scenario PU20bx25: global tag is PHYS14_25_V1
#  as a rule, find the global tag in the DAS under the Configs for given dataset
process.GlobalTag.globaltag = 'PHYS14_25_V1'

#
# Define input data to read
#
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(
   #
   # Just a handful of files from the dataset are listed below, for testing
   #
       '/store/mc/Phys14DR/DYJetsToLL_M-50_13TeV-madgraph-pythia8/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/0432E62A-7A6C-E411-87BB-002590DB92A8.root',
      '/store/mc/Phys14DR/DYJetsToLL_M-50_13TeV-madgraph-pythia8/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/06C61714-7E6C-E411-9205-002590DB92A8.root',
       '/store/mc/Phys14DR/DYJetsToLL_M-50_13TeV-madgraph-pythia8/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/0EAD09A8-7C6C-E411-B903-0025901D493E.root'
 ),
              #               eventsToProcess = cms.untracked.VEventRange("1:2862883-1:2862883")
                             
)



#
# START ELECTRON ID SECTION
#
# Set up everything that is needed to compute electron IDs and
# add the ValueMaps with ID decisions into the event data stream
#

# Load tools and function definitions
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *

process.load("RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cfi")
# overwrite a default parameter: for miniAOD, the collection name is a slimmed one
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectrons')

from PhysicsTools.SelectorUtils.centralIDRegistry import central_id_registry
process.egmGsfElectronIDSequence = cms.Sequence(process.egmGsfElectronIDs)

# Define which IDs we want to produce
# Each of these two example IDs contains all four standard 
# cut-based ID working points (only two WP of the PU20bx25 are actually used here).
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V0_miniAOD_cff']
#Add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

#heep works differently because mini-aod and aod are defined in the same file to ensure consistancy (you cant change cuts of aod without changing miniaod
process.load('RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV51_cff')
setupVIDSelection(process.egmGsfElectronIDs,process.heepElectronID_HEEPV51_miniAOD)

# Do not forget to add the egmGsfElectronIDSequence to the path,
# as in the example below!

#
# END ELECTRON ID SECTION
#

#
# Configure an example module for user analysis of electrons
#
process.load("RecoEgamma.ElectronIdentification.ElectronIDValueMapProducer_cfi")

process.electronIDValueMapProducer.ebReducedRecHitCollection=cms.InputTag("reducedEgamma","reducedEBRecHits")
process.electronIDValueMapProducer.eeReducedRecHitCollection=cms.InputTag("reducedEgamma","reducedEERecHits")
process.electronIDValueMapProducer.esReducedRecHitCollection=cms.InputTag("reducedEgamma","reducedESRecHits")
process.electronIDValueMapProducer.src=cms.InputTag("slimmedElectrons")
process.electronIDValueMapProducer.datasetFormat=cms.string("PAT")

## process.load("Configuration.StandardSequences.MagneticField_cff")
## process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
## process.load("SHarper.SHNtupliser.shNtupliser_cfi")
## process.shNtupliser.fillFromGsfEle=False
## process.shNtupliser.electronTag = cms.untracked.InputTag("slimmedElectrons")
## process.shNtupliser.superClusterEBTag =cms.InputTag("reducedEgamma","reducedSuperClusters")
## process.shNtupliser.superClusterEETag =cms.InputTag("reducedEgamma","reducedSuperClusters")


process.idDemo = cms.EDAnalyzer('IDTester',
     electrons = cms.InputTag("slimmedElectrons"),
     heepIdMap = cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV51-miniAOD"),
)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('electron_ntuple_mini.root')
                                   )

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
process.load("SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi")
from SHarper.HEEPAnalyzer.HEEPAnalyzer_cfi import swapHEEPToMiniAOD
swapHEEPToMiniAOD(process.heepAnalyzer)

process.p = cms.Path(process.electronIDValueMapProducer * process.egmGsfElectronIDSequence * process.heepAnalyzer)
