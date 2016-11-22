isMC=True
useMiniAOD=True

# Import configurations
import FWCore.ParameterSet.Config as cms


# set up process
process = cms.Process("HEEP")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(10000000)
)
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')

#setup global tag
from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.AlCa.autoCond import autoCond
if isMC:
    #process.GlobalTag.globaltag = autoCond['run2_mc']
    process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_TrancheIV_v4', '') 
else:
    #process.GlobalTag.globaltag = autoCond['run2_data']
    process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016SeptRepro_v4', '')


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(
        '/store/mc/RunIISpring16DR80/ZToEE_NNPDF30_13TeV-powheg_M_200_400/RAWAODSIM/PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1/50000/FAB476A3-4F13-E611-B591-008CFAF06402.root',
)
                                       
)

if useMiniAOD==True:
    process.source.fileNames=cms.untracked.vstring('file:/opt/ppd/scratch/harper/mcTestFiles/ZToEE_NNPDF30_13TeV-powheg_M_200_400_MINIAODSIM_PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1_0C242E56-BA3A-E611-9B2D-0242AC130004.root',)

#setup the VID with HEEP 7.0, not necessary if you dont want to use VID
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate
if useMiniAOD:
    switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
else:
    switchOnVIDElectronIdProducer(process, DataFormat.AOD)

# define which IDs we want to produce
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff']
#add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)


#this is our example analysis module reading the results
process.heepIdVIDComp = cms.EDAnalyzer("HEEPV70Example",
                                       elesAOD=cms.InputTag("gedGsfElectrons"),
                                       elesMiniAOD=cms.InputTag("slimmedElectrons"),
                                       trkIsolMap=cms.InputTag("heepIDVarValueMaps","eleTrkPtIso"),
                                       vid=cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV70")
                                       )

process.p = cms.Path(
    process.egmGsfElectronIDSequence* #makes the VID value maps, only necessary if you use VID
    process.heepIdVIDComp) #our analysing example module, replace with your module

#dumps the products made for easier debugging
process.load('Configuration.EventContent.EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('MINIAODSIM'),
        filterName = cms.untracked.string('')
    ),
    dropMetaData = cms.untracked.string('ALL'),
    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
    fastCloning = cms.untracked.bool(False),
    fileName = cms.untracked.string('outputTest.root'),
    outputCommands = process.MINIAODSIMEventContent.outputCommands,
    overrideInputFileSplitLevels = cms.untracked.bool(True)
)
process.output.outputCommands = cms.untracked.vstring('keep *_*_*_*',
                                                           )
process.outPath = cms.EndPath(process.output)
