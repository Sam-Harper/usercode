import FWCore.ParameterSet.Config as cms

#here is an example of how to produce depth 1 and depth 2 hcal isolations from
#calotowers
#the hcal depth 1 has a inner veto cone of 0.1 while depth 2 has no veto cone


#need the EleIsoHcalFromTowersExtractorBlock to configure the CandIsoDepositProducer
#the defaults are sensible
from RecoEgamma.EgammaIsolationAlgos.eleHcalExtractorBlocks_cff import *


##############################################################################
##Step 1: produce IsoDeposits from CaloTowers using the HCAL depth 1        ##
##        and depth 2 energies                                              ##
##############################################################################
eleIsoDepositHcalDepth1FromTowers = cms.EDProducer("CandIsoDepositProducer",
    src = cms.InputTag("pixelMatchGsfElectrons"),
    trackType = cms.string('candidate'),
    MultipleDepositsFlag = cms.bool(False),
    ExtractorPSet = cms.PSet(EleIsoHcalFromTowersExtractorBlock)
)
#we want to specify depth 1 only so we reconfigure the hcal depth parameter
eleIsoDepositHcalDepth1FromTowers.hcalDepth = cms.int32(1)

eleIsoDepositHcalDepth2FromTowers = cms.EDProducer("CandIsoDepositProducer",
    src = cms.InputTag("pixelMatchGsfElectrons"),
    trackType = cms.string('candidate'),
    MultipleDepositsFlag = cms.bool(False),
    ExtractorPSet = cms.PSet(EleIsoHcalFromTowersExtractorBlock)
)
#we want to specify depth 2 only so we reconfigure the hcal depth parameter
eleIsoDepositHcalDepth2FromTowers.hcalDepth = cms.int32(2)


##############################################################################
##Step 2: produce ValueMaps containing the isolation from the IsoDeposits   ##
##        produced by step 1). The input tag of of the ValueMap is the name ##
##        of the module, eg eleIsoFromDepsHcalDepth1FromTowers              ##
##############################################################################

eleIsoFromDepsHcalDepth1FromTowers = cms.EDFilter("CandIsolatorFromDeposits",
    deposits = cms.VPSet(cms.PSet(
        src = cms.InputTag("eleIsoDepositHcalDepth1FromTowers"), #the input isodeposits
        deltaR = cms.double(0.4), #outer cone size
        weight = cms.string('1'),
        vetos = cms.vstring('0.1'), # the veto (here vetoing inner cone of 0.1)
        skipDefaultVeto = cms.bool(True), #default veto is depreciated and usually skipped, using the vetoes specifed in veto above
        mode = cms.string('sum') #sum the Ets
    ))
)

eleIsoFromDepsHcalDepth2FromTowers = cms.EDFilter("CandIsolatorFromDeposits",
    deposits = cms.VPSet(cms.PSet(
        src = cms.InputTag("eleIsoDepositHcalDepth2FromTowers"),
        deltaR = cms.double(0.4),
        weight = cms.string('1'),
        vetos = cms.vstring('0.0'),
        skipDefaultVeto = cms.bool(True),
        mode = cms.string('sum')
    ))
)


#define a sequence, produce the IsoDeposits first and then make the isolations from them
eleHcalDepthSplitIsolSequence = cms.Sequence(eleIsoDepositHcalDepth1FromTowers*eleIsoFromDepsHcalDepth1FromTowers*eleIsoDepositHcalDepth2FromTowers*eleIsoFromDepsHcalDepth2FromTowers) 
