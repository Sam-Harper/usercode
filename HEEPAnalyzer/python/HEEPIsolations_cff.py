import FWCore.ParameterSet.Config as cms

#because e/gamma isolation was a bit of a mess and the pat isnt upto date
#this file effectively replaces PhysicsTools/PatAlgos/python/recoLayer0/electronIsolation_cff.py


#this is a list of all the isolation deposits we need to convert to value maps
#so pat can use them
#step 1) get a list of isolations for which we need to do this
#step 2) convert the isoDeposits to value maps
#step 3) associate the value maps to the pat layer 0 output

patHEEPElectronIsolationLabels = cms.VInputTag(
        cms.InputTag("eleIsoDepositEcalFromHits"),
        cms.InputTag("eleIsoDepositTk"),
        cms.InputTag("eleIsoDepositHcalFromTowers"),
        cms.InputTag("eleIsoDepositHcalDepth1FromTowers"),
        cms.InputTag("eleIsoDepositHcalDepth2FromTowers")
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


patLayer0HEEPIsolSequence = cms.Sequence(patHEEPElectronIsolationMaps*
                                         patLayer0HEEPElectronIsolations)

#now in theory if we run this sequence before the pat electron producer, we should be able
#to just load these isolations into  the pat electron
