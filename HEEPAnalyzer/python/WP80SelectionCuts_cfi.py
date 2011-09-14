import FWCore.ParameterSet.Config as cms

#this is where the WP80 selection is defined

wp80BarrelCuts = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks:isolPtTrksRel03:isolEmRel03:isolHadRel03"),
    minEt=cms.double(25),
    minEta=cms.double(0.),
    maxEta=cms.double(1.442),
    maxDEtaIn=cms.double(0.004), #new value for WP80
    maxDPhiIn=cms.double(0.06),  #new value for WP80
    maxHadem=cms.double(0.04),   #new value for WP80
    maxSigmaIEtaIEta=cms.double(0.01), # new value for WP80
    minE2x5Over5x5=cms.double(0.94),
    minE1x5Over5x5=cms.double(0.83),
    isolEmHadDepth1ConstTerm=cms.double(2), #cut is of the form <const + grad*(et-gradStart) where et-gradStart is 0 if negative
    isolEmHadDepth1GradTerm=cms.double(0.03),
    isolEmHadDepth1GradStart=cms.double(0.),
    isolHadDepth2ConstTerm=cms.double(999.), #not defined for barrel
    isolHadDepth2GradTerm=cms.double(0.0), #not defined for barrel
    isolHadDepth2GradStart=cms.double(0.), #not defined for barrel
    isolPtTrksConstTerm=cms.double(7.5),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    maxIsolPtTrksRel03=cms.double(0.09),  #WP80
    maxIsolEmRel03=cms.double(0.07),      #WP80
    maxIsolHadRel03=cms.double(0.1),      #WP80

)

wp80EndcapCuts = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolHadDepth2:isolPtTrks:isolPtTrksRel03:isolEmRel03:isolHadRel03"),
    minEt=cms.double(25),
    minEta=cms.double(1.56),
    maxEta=cms.double(2.5),
    maxDEtaIn=cms.double(0.007), # same value for WP80
    maxDPhiIn=cms.double(0.03),  # new value for WP80
    maxHadem=cms.double(0.025),   # new value for WP80
    maxSigmaIEtaIEta=cms.double(0.03), #same value for WP80
    minE2x5Over5x5=cms.double(0.), #not defined for endcap
    minE1x5Over5x5=cms.double(0.), #not defined for endcap
    isolEmHadDepth1ConstTerm=cms.double(2.5),
    isolEmHadDepth1GradTerm=cms.double(0.03),
    isolEmHadDepth1GradStart=cms.double(50.),
    isolHadDepth2ConstTerm=cms.double(0.5),
    isolHadDepth2GradTerm=cms.double(0.0),
    isolHadDepth2GradStart=cms.double(0.),
    isolPtTrksConstTerm=cms.double(15),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    maxIsolPtTrksRel03=cms.double(0.04),   #WP80
    maxIsolEmRel03=cms.double(0.05),       #WP80
    maxIsolHadRel03=cms.double(0.025),     #WP80

)
