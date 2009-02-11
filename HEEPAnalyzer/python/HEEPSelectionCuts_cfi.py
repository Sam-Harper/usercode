import FWCore.ParameterSet.Config as cms

heepBarrelCuts = cms.PSet (
    cuts=cms.string("et:detEta:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks"),
    validEleTypes=cms.string("barrel:golden:showering:bigBrem:narrow:crack"),
    minEt=cms.double(25),
    minEta=cms.double(0.),
    maxEta=cms.double(1.442),
    maxDEtaIn=cms.double(0.005),
    maxDPhiIn=cms.double(0.09),
    maxHadem=cms.double(0.05),
    maxSigmaIEtaIEta=cms.double(999.), #not defined for barrel
    minE2x5Over5x5=cms.double(0.94),
    minE1x5Over5x5=cms.double(0.83),
    isolEmHadDepth1ConstTerm=cms.double(3), #cut is of the form <const + grad*(et-gradStart) where et-gradStart is 0 if negative
    isolEmHadDepth1GradTerm=cms.double(0.02),
    isolEmHadDepth1GradStart=cms.double(0.),
    isolHadDepth2ConstTerm=cms.double(999.), #not defined for barrel
    isolHadDepth2GradTerm=cms.double(0.0), #not defined for barrel
    isolHadDepth2GradStart=cms.double(0.), #not defined for barrel
    isolPtTrksConstTerm=cms.double(7.5),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    isolNrTrksConstTerm=cms.int32(4) #not defined for barrel
)

heepEndcapCuts = cms.PSet (
    cuts=cms.string("et:detEta:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolHadDepth2:isolPtTrks"),
    validEleTypes=cms.string("endcap:golden:showering:bigBrem:narrow:crack"),
    minEt=cms.double(25),
    minEta=cms.double(1.56),
    maxEta=cms.double(2.5),
    maxDEtaIn=cms.double(0.007),
    maxDPhiIn=cms.double(0.09),
    maxHadem=cms.double(0.05),
    maxSigmaIEtaIEta=cms.double(0.0275),
    minE2x5Over5x5=cms.double(0.), #not defined for endcap
    minE1x5Over5x5=cms.double(0.), #not defined for endcap
    isolEmHadDepth1ConstTerm=cms.double(5.5),
    isolEmHadDepth1GradTerm=cms.double(0.05),
    isolEmHadDepth1GradStart=cms.double(50.),
    isolHadDepth2ConstTerm=cms.double(0.5),
    isolHadDepth2GradTerm=cms.double(0.0),
    isolHadDepth2GradStart=cms.double(0.),
    isolPtTrksConstTerm=cms.double(15),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    isolNrTrksConstTerm=cms.int32(4) #not defined for barrel

)
