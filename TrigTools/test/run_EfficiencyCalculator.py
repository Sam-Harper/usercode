import FWCore.ParameterSet.Config as cms
import glob
import os

process = cms.Process("USER")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '123X_mcRun3_2021_realistic_v14') #GT including the latest electron energy regression

if 'GlobalTag' in process.__dict__:
        #note could probably just override the tag of the pset to the correct value
        #rather than removing...
        for indx,pset in enumerate(process.GlobalTag.toGet):
            if pset.record.value()=='L1TUtmTriggerMenuRcd':
                process.GlobalTag.toGet.pop(indx)
        from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
        process.GlobalTag = customiseGlobalTag(process.GlobalTag, conditions = 'L1Menu_Collisions2018_v2_1_0-d1_xml,L1TUtmTriggerMenuRcd,,,9999-12-31 23:59:59.000')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

dirName = "/eos/cms/store/group/phys_egamma/swmukher/hlt_12_3/WithCaloTower/ZprimeToEE_M-6000_TuneCP5_14TeV-pythia8/crab_crab_ZprimeToEE_1/220129_064015/0000/"
fileList = filter(os.path.isfile, glob.glob(dirName + "output_*.root"))
fList = []
for f in fileList:
    fs = str(f).replace("/eos/","file:/eos/")
    fList.append(fs)

process.source = cms.Source("PoolSource",
                                fileNames = cms.untracked.vstring(fList
                                    #'root://cms-xrd-global.cern.ch//store/data/Run2016B/SingleElectron/MINIAOD/17Jul2018_ver2-v1/80000/FEAB7D8A-048C-E811-A513-AC1F6B1AEFFC.root'
            #'file:/eos/cms/store/group/phys_egamma/swmukher/hlt_12_3/WithCaloTower/ZprimeToEE_M-6000_TuneCP5_14TeV-pythia8/crab_crab_ZprimeToEE_1/220129_064015/0000/output_192.root'
                )
                            )

#Output file
process.TFileService = cms.Service("TFileService",
   fileName = cms.string("EfficiencyDRnew_noEG26.root")
)

process.hltEgammaCandidates = cms.EDProducer( "EgammaHLTRecoEcalCandidateProducers",
    scHybridBarrelProducer = cms.InputTag( 'hltParticleFlowSuperClusterECALL1Seeded','hltParticleFlowSuperClusterECALBarrel' ),
    scIslandEndcapProducer = cms.InputTag( 'hltParticleFlowSuperClusterECALL1Seeded','hltParticleFlowSuperClusterECALEndcapWithPreshower' ),
    recoEcalCandidateCollection = cms.string( "" )
)

process.GlobalParametersRcdSource = cms.ESSource("EmptyESSource",
    recordName = cms.string('L1TGlobalParametersRcd'),
    iovIsRunNotTime = cms.bool(True),
    firstValid = cms.vuint32(1)
)

process.GlobalParameters = cms.ESProducer("StableParametersTrivialProducer", 
    # bx in event
    #NumberBxInEvent = cms.int32(5),

    # trigger decision
    
    # number of physics trigger algorithms
    NumberPhysTriggers = cms.uint32(512),


    # trigger objects

    # muons
    NumberL1Muon = cms.uint32(8),
    
    # e/gamma and isolated e/gamma objects
    NumberL1EGamma = cms.uint32(12),

    #  jets
    NumberL1Jet = cms.uint32(12),

    # taus
    NumberL1Tau = cms.uint32(12),

    # hardware

    # number of maximum chips defined in the xml file
    NumberChips = cms.uint32(1),

    # number of pins on the GTL condition chips
    PinsOnChip = cms.uint32(512),

    # correspondence "condition chip - GTL algorithm word" in the hardware
    # e.g.: chip 2: 0 - 95;  chip 1: 96 - 128 (191)
    OrderOfChip = cms.vint32(1),
)

process.hltGtStage2ObjectMap = cms.EDProducer( "L1TGlobalProducer",
    MuonInputTag = cms.InputTag( 'hltGtStage2Digis','Muon' ),
    MuonShowerInputTag = cms.InputTag( 'hltGtStage2Digis','MuonShower' ),
    EGammaInputTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    TauInputTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    JetInputTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    EtSumInputTag = cms.InputTag( 'hltGtStage2Digis','EtSum' ),
    ExtInputTag = cms.InputTag( "hltGtStage2Digis" ),
    AlgoBlkInputTag = cms.InputTag( "hltGtStage2Digis" ),
    GetPrescaleColumnFromData = cms.bool( False ),
    AlgorithmTriggersUnprescaled = cms.bool( True ),
    RequireMenuToMatchAlgoBlkInput = cms.bool( True ),
    AlgorithmTriggersUnmasked = cms.bool( True ),
    useMuonShowers = cms.bool( True ),
    #resetPSCountersEachLumiSec = cms.bool( True ), #it looks like this parameter doesn't exist in the CMSSW release used
    ProduceL1GtDaqRecord = cms.bool( True ),
    ProduceL1GtObjectMapRecord = cms.bool( True ),
    EmulateBxInEvent = cms.int32( 1 ),
    L1DataBxInEvent = cms.int32( 5 ),
    AlternativeNrBxBoardDaq = cms.uint32( 0 ),
    BstLengthBytes = cms.int32( -1 ),
    PrescaleSet = cms.uint32( 1 ),
    Verbosity = cms.untracked.int32( 0 ),
    PrintL1Menu = cms.untracked.bool( False ),
    TriggerMenuLuminosity = cms.string( "startup" )
)

process.hltL1sSingleEGor = cms.EDFilter( "HLTL1TSeed",
    saveTags = cms.bool( True ),
    #L1SeedsLogicalExpression = cms.string( "L1_SingleLooseIsoEG26er2p5 OR L1_SingleLooseIsoEG26er1p5 OR L1_SingleLooseIsoEG28er2p5 OR L1_SingleLooseIsoEG28er2p1 OR L1_SingleLooseIsoEG28er1p5 OR L1_SingleLooseIsoEG30er2p5 OR L1_SingleLooseIsoEG30er1p5 OR L1_SingleEG26er2p5 OR L1_SingleEG38er2p5 OR L1_SingleEG40er2p5 OR L1_SingleEG42er2p5 OR L1_SingleEG45er2p5 OR L1_SingleEG60 OR L1_SingleEG34er2p5 OR L1_SingleEG36er2p5 OR L1_SingleIsoEG24er2p1 OR L1_SingleIsoEG26er2p1 OR L1_SingleIsoEG28er2p1 OR L1_SingleIsoEG30er2p1 OR L1_SingleIsoEG32er2p1 OR L1_SingleIsoEG26er2p5 OR L1_SingleIsoEG28er2p5 OR L1_SingleIsoEG30er2p5 OR L1_SingleIsoEG32er2p5 OR L1_SingleIsoEG34er2p5"),
        L1SeedsLogicalExpression = cms.string( "L1_SingleLooseIsoEG28er2p5 OR L1_SingleLooseIsoEG28er2p1 OR L1_SingleLooseIsoEG28er1p5 OR L1_SingleLooseIsoEG30er2p5 OR L1_SingleLooseIsoEG30er1p5 OR L1_SingleEG38er2p5 OR L1_SingleEG40er2p5 OR L1_SingleEG42er2p5 OR L1_SingleEG45er2p5 OR L1_SingleEG60 OR L1_SingleEG34er2p5 OR L1_SingleEG36er2p5 OR L1_SingleIsoEG26er2p1 OR L1_SingleIsoEG28er2p1 OR L1_SingleIsoEG30er2p1 OR L1_SingleIsoEG32er2p1 OR L1_SingleIsoEG26er2p5 OR L1_SingleIsoEG28er2p5 OR L1_SingleIsoEG30er2p5 OR L1_SingleIsoEG32er2p5 OR L1_SingleIsoEG34er2p5"),
        #L1SeedsLogicalExpression = cms.string( "L1_SingleLooseIsoEG30er2p5 OR L1_SingleLooseIsoEG30er1p5 OR L1_SingleEG38er2p5 OR L1_SingleEG40er2p5 OR L1_SingleEG42er2p5 OR L1_SingleEG45er2p5 OR L1_SingleEG60 OR L1_SingleEG34er2p5 OR L1_SingleEG36er2p5 OR L1_SingleIsoEG30er2p1 OR L1_SingleIsoEG32er2p1 OR L1_SingleIsoEG30er2p5 OR L1_SingleIsoEG32er2p5 OR L1_SingleIsoEG34er2p5"),
    #L1SeedsLogicalExpression = cms.string( "L1_SingleLooseIsoEG30er1p5 OR L1_SingleEG38er2p5 OR L1_SingleEG40er2p5 OR L1_SingleEG42er2p5 OR L1_SingleEG45er2p5 OR L1_SingleEG60 OR L1_SingleEG34er2p5 OR L1_SingleEG36er2p5 OR L1_SingleIsoEG30er2p1 OR L1_SingleIsoEG32er2p1 OR L1_SingleIsoEG30er2p5 OR L1_SingleIsoEG32er2p5 OR L1_SingleIsoEG34er2p5"), #opt4
    L1ObjectMapInputTag = cms.InputTag( "hltGtStage2ObjectMap" ),
    L1GlobalInputTag = cms.InputTag( "hltGtStage2Digis" ),
    L1MuonInputTag = cms.InputTag( 'hltGtStage2Digis','Muon' ),
    L1MuonShowerInputTag = cms.InputTag( 'hltGtStage2Digis','MuonShower' ),
    L1EGammaInputTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    L1JetInputTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    L1TauInputTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    L1EtSumInputTag = cms.InputTag( 'hltGtStage2Digis','EtSum' )
)

process.hltL1sSingleEGorFilter = cms.EDFilter( "HLTEgammaL1TMatchFilterRegional",
    saveTags = cms.bool( True ),
    candIsolatedTag = cms.InputTag( "hltEgammaCandidates" ),
    l1IsolatedTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    candNonIsolatedTag = cms.InputTag( "" ),
    l1NonIsolatedTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    L1SeedFilterTag = cms.InputTag( "hltL1sSingleEGor" ),
    l1CenJetsTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    l1TausTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    ncandcut = cms.int32( 1 ),
    doIsolated = cms.bool( False ),
    region_eta_size = cms.double( 0.522 ),
    region_eta_size_ecap = cms.double( 1.0 ),
    region_phi_size = cms.double( 1.044 ),
    barrel_end = cms.double( 1.4791 ),
    endcap_end = cms.double( 2.65 )
)

#process.load("SHarper.TrigTools.EfficiencyCalculator_cfi")
process.EfficiencyCalculator = cms.EDAnalyzer('EfficiencyCalculator'
                              )
#process.p = cms.Path(process.EfficiencyCalculator)
process.myTask = cms.Task(process.GlobalParametersRcdSource,process.GlobalParameters,process.hltGtStage2ObjectMap)
process.t = cms.Path(process.myTask)
#process.p = cms.Path(process.hltL1sSingleEGor + process.hltL1sSingleEGorFilter + process.EfficiencyCalculator)
process.p = cms.Path(process.hltL1sSingleEGor + process.EfficiencyCalculator)
process.schedule = cms.Schedule(process.t,process.p)
#process.mySequence = cms.Schedule(process.hltGtStage2ObjectMap + process.hltL1sSingleEGor + process.hltL1sSingleEGorFilter + process.EfficiencyCalculator)
