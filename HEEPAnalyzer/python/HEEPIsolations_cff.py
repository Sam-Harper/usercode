import FWCore.ParameterSet.Config as cms

#from RecoEgamma.EgammaIsolationAlgos.eleIsoDeposits_cff import *

from RecoEgamma.EgammaIsolationAlgos.eleHcalExtractorBlocks_cff import *

#so the first section is just defining the modules to create our isolation, identical to how you would
#do it in a normal non pat way
#this bit is commented out as already defined in previous configs
#eleIsoDepositHcalFromTowers = cms.EDProducer("CandIsoDepositProducer",
#    src = cms.InputTag("pixelMatchGsfElectrons"),
#    MultipleDepositsFlag = cms.bool(False),
#    trackType = cms.string('candidate'),
#    ExtractorPSet = cms.PSet( EleIsoHcalFromTowersExtractorBlock )
#)



eleIsoDepositHcalFromTowersDepth1 = cms.EDProducer("CandIsoDepositProducer",
    src = cms.InputTag("pixelMatchGsfElectrons"),
    MultipleDepositsFlag = cms.bool(False),
    trackType = cms.string('candidate'),
    ExtractorPSet = cms.PSet( EleIsoHcalFromTowersExtractorBlock )
)
eleIsoDepositHcalFromTowersDepth1.ExtractorPSet.hcalDepth = cms.int32(1)

eleIsoDepositHcalFromTowersDepth2 = cms.EDProducer("CandIsoDepositProducer",
    src = cms.InputTag("pixelMatchGsfElectrons"),
    MultipleDepositsFlag = cms.bool(False),
    trackType = cms.string('candidate'),
    ExtractorPSet = cms.PSet( EleIsoHcalFromTowersExtractorBlock )
)
eleIsoDepositHcalFromTowersDepth2.ExtractorPSet.hcalDepth = cms.int32(2)

#this is a list of all the isolation deposits we need to convert to value maps
#so pat can use them
#step 1) get a list of isolations for which we need to do this
#step 2) convert the isoDeposits to value maps
#step 3) associate the value maps to the pat layer 0 output

patHEEPElectronIsolationLabels = cms.VInputTag(
        cms.InputTag("eleIsoDepositEcalFromHits"),
        cms.InputTag("eleIsoDepositHcalFromTowersDepth1"),
        cms.InputTag("eleIsoDepositHcalFromTowersDepth2")
)

# read and convert to ValueMap<IsoDeposit> keyed to Candidate
patHEEPElectronIsolationMaps = cms.EDFilter("MultipleIsoDepositsToValueMaps",
    collection   = cms.InputTag("pixelMatchGsfElectrons"),
    associations = patHEEPElectronIsolationLabels,
)

#pat currently (219) needs the isolation deposits in a value map keyed to the layer0 output
#this does this
patLayer0HEEPElectronIsolations = cms.EDFilter("CandManyValueMapsSkimmerIsoDeposits",
    collection   = cms.InputTag("allLayer0Electrons"),
    backrefs     = cms.InputTag("allLayer0Electrons"),
    commonLabel  = cms.InputTag("patHEEPElectronIsolationMaps"),
    associations = patHEEPElectronIsolationLabels,
)

#now we need a sequence to actually run
#patLayer0HEEPIsolSequence = cms.Sequence(eleIsoDepositEcalFromHits*eleIsoDepositHcalFromTowersDepth1*
#                                         eleIsoDepositHcalFromTowersDepth2)
heepIsolSequence = cms.Sequence(eleIsoDepositHcalFromTowersDepth1*
                                         eleIsoDepositHcalFromTowersDepth2)

patLayer0HEEPIsolSequence = cms.Sequence(heepIsolSequence*
                                         patHEEPElectronIsolationMaps*
                                         patLayer0HEEPElectronIsolations)

#now in theory if we run this sequence before the pat electron producer, we should be able
#to just load these isolations into  the pat electron
