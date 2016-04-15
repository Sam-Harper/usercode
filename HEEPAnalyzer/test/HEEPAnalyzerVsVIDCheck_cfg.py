import FWCore.ParameterSet.Config as cms

process = cms.Process("TestID")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(1000),
    limit = cms.untracked.int32(10000000)
)

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
# NOTE: the pick the right global tag!
#    for Spring15 50ns MC: global tag is 'auto:run2_mc_50'
#    for Spring15 25ns MC: global tag is 'auto:run2_mc'
#    for Run 2 data: global tag is 'auto:run2_data'
#  as a rule, find the "auto" global tag in $CMSSW_RELEASE_BASE/src/Configuration/AlCa/python/autoCond.py
#  This auto global tag will look up the "proper" global tag
#  that is typically found in the DAS under the Configs for given dataset
#  (although it can be "overridden" by requirements of a given release)
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

#
# Define input data to read
#
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


inputFilesAOD = cms.untracked.vstring(
    # AOD test files from /DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/AODSIM
       '/store/mc/RunIISpring15DR74/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/AODSIM/Asympt25ns_MCRUN2_74_V9-v3/10000/002F7FDD-BA13-E511-AA63-0026189437F5.root',
       '/store/mc/RunIISpring15DR74/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/AODSIM/Asympt25ns_MCRUN2_74_V9-v3/10000/00610EE7-C213-E511-842C-00304833529A.root',
       '/store/mc/RunIISpring15DR74/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/AODSIM/Asympt25ns_MCRUN2_74_V9-v3/10000/00623DCC-A813-E511-A302-0025905B85A2.root',
    )    

inputFilesMiniAOD = cms.untracked.vstring(
    # MiniAOD test files from /DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/MINIAODSIM
#    '/store/mc/RunIIFall15MiniAODv1/ZToEE_NNPDF30_13TeV-powheg_M_2300_3500/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/10000/201866B5-EBB0-E511-9D50-008CFAF06942.root',
    '/store/mc/RunIIFall15MiniAODv2/ZToEE_NNPDF30_13TeV-powheg_M_2300_3500/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/60000/84571177-6AB8-E511-B503-002590E505FE.root',
    '/store/mc/RunIIFall15MiniAODv2/ZToEE_NNPDF30_13TeV-powheg_M_200_400/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/28640CCE-8FB8-E511-B8FC-001EC9ADE609.root',
    '/store/mc/RunIIFall15MiniAODv2/ZToEE_NNPDF30_13TeV-powheg_M_50_120/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/80000/1E4876E3-05D5-E511-85B9-44A84225C7BB.root',
    '/store/mc/RunIIFall15MiniAODv2/ZToEE_NNPDF30_13TeV-powheg_M_50_120/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/80000/20CB3216-05D5-E511-851E-002590D8C740.root',
    )

# Set up input/output depending on the format
# You can list here either AOD or miniAOD files, but not both types mixed
#
useAOD = False

if useAOD == True :
    inputFiles = inputFilesAOD
    outputFile = "electron_ntuple.root"
    print("AOD input files are used")
else :
    inputFiles = inputFilesMiniAOD
    outputFile = "electron_ntuple_mini.root"
    print("MiniAOD input files are used")
process.source = cms.Source ("PoolSource", fileNames = inputFiles )                             

#
# Set up electron ID (VID framework)
#

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate 
if useAOD == True :
    dataFormat = DataFormat.AOD
else :
    dataFormat = DataFormat.MiniAOD

switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff',
                 'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV60_cff',
                 'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV61_cff']

#add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('electron_ntuple_mini.root')
                                   )


#setting up the producer to make the HEEP ID value map
from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *
process.heepId = cms.EDProducer("HEEPIdValueMapProducer",
                                eleLabel = cms.InputTag("gedGsfElectrons"),
                                barrelCuts = cms.PSet(heepBarrelCuts),
                                endcapCuts = cms.PSet(heepEndcapCuts),
                                eleIsolEffectiveAreas = cms.PSet(heepEffectiveAreas),
                                eleRhoCorrLabel = cms.InputTag("fixedGridRhoFastjetAll"),
                                applyRhoCorrToEleIsol = cms.bool(True),
                                verticesLabel = cms.InputTag("offlinePrimaryVertices"),
                                writeIdAsInt =cms.bool(True) #true saves the heep ID as an int, false: saves as a float, user request
                                )
process.heepIdVIDComp = cms.EDAnalyzer("HEEPIdAndVIDComp",
                                       eleLabel=cms.InputTag("gedGsfElectrons"),
                                       heepId=cms.InputTag("heepId"),
                                       vid=cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV61"),
                                       rho = cms.InputTag("fixedGridRhoFastjetAll")
                                       )
if useAOD==False:
    process.heepId.verticesLabel=cms.InputTag("offlineSlimmedPrimaryVertices")
    process.heepId.eleLabel=cms.InputTag("slimmedElectrons")
    process.heepIdVIDComp.eleLabel=cms.InputTag("slimmedElectrons")
 


process.p = cms.Path(process.heepId* #makes the HEEPID value map
                     process.egmGsfElectronIDSequence* #makes the VID value maps
                     process.heepIdVIDComp
                     )
