import FWCore.ParameterSet.Config as cms

#this is where the HEEP selection is defined

heepBarrelCutsV41 = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks:nrMissHits:dxy"),
    minEt=cms.double(35),
    minEta=cms.double(0.),
    maxEta=cms.double(1.442),
    maxDEtaIn=cms.double(0.005),
    dEtaInConstTerm =cms.double(0),
    dEtaInGradTerm =cms.double(0),
    maxDPhiIn=cms.double(0.06),
    maxHadem=cms.double(0.05),
    hademConstTerm=cms.double(0),
    maxSigmaIEtaIEta=cms.double(999.), #not defined for barrel
    minE2x5Over5x5=cms.double(0.94),
    minE1x5Over5x5=cms.double(0.83),
    maxNrSatCrysForShowerShapeCut=cms.int32(26),
    isolEmHadDepth1ConstTerm=cms.double(2), #cut is of the form <const + grad*(et-gradStart) where et-gradStart is 0 if negative
    isolEmHadDepth1GradTerm=cms.double(0.03),
    isolEmHadDepth1GradStart=cms.double(0.),
    isolEmHadDepth1RhoEA=cms.double(0.28),
    isolHadDepth2ConstTerm=cms.double(999.), #not defined for barrel
    isolHadDepth2GradTerm=cms.double(0.0), #not defined for barrel
    isolHadDepth2GradStart=cms.double(0.), #not defined for barrel
    isolHadDepth2RhoEA=cms.double(0.), #not defined for barrel
    isolPtTrksConstTerm=cms.double(5.0),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    isolPtTrksRhoStart=cms.double(99999999.),
    isolPtTrksRhoEA=cms.double(0.),
    useValMapForTrkIso=cms.bool(False),
    maxIsolPtTrksRel03=cms.double(0.09),  #WP80
    isolPtTrksRelRhoEA=cms.double(0.),
    maxIsolEmRel03=cms.double(0.07),      #WP80
    isolEmRelRhoEA=cms.double(0.14),
    maxIsolHadRel03=cms.double(0.1),      #WP80
    isolHadRelRhoEA=cms.double(0.14),
    maxNrMissHits=cms.int32(1),
    maxDXY=cms.double(0.02)
)

heepEndcapCutsV41 = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolPtTrks:nrMissHits:dxy"),
    minEt=cms.double(35),
    minEta=cms.double(1.56),
    maxEta=cms.double(2.5),
    maxDEtaIn=cms.double(0.007),
    dEtaInConstTerm =cms.double(0),
    dEtaInGradTerm =cms.double(0),
    maxDPhiIn=cms.double(0.06),
    maxHadem=cms.double(0.05),
    hademConstTerm=cms.double(0),
    maxSigmaIEtaIEta=cms.double(0.03),
    minE2x5Over5x5=cms.double(0.), #not defined for endcap
    minE1x5Over5x5=cms.double(0.), #not defined for endcap
    maxNrSatCrysForShowerShapeCut=cms.int32(26),
    isolEmHadDepth1ConstTerm=cms.double(2.5),
    isolEmHadDepth1GradTerm=cms.double(0.03),
    isolEmHadDepth1GradStart=cms.double(50.),
    isolEmHadDepth1RhoEA=cms.double(0.28),
    isolHadDepth2ConstTerm=cms.double(999), #no longer defined for endcap
    isolHadDepth2GradTerm=cms.double(0.0),#no longer defined for endcap
    isolHadDepth2GradStart=cms.double(0.),#no longer defined for endcap
    isolHadDepth2RhoEA=cms.double(0.),#no longer defined for endcap
    isolPtTrksConstTerm=cms.double(5.0),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    isolPtTrksRhoStart=cms.double(99999999.),
    isolPtTrksRhoEA=cms.double(0.), 
    useValMapForTrkIso=cms.bool(False),
    maxIsolPtTrksRel03=cms.double(0.09),  #WP80
    isolPtTrksRelRhoEA=cms.double(0.),
    maxIsolEmRel03=cms.double(0.07),      #WP80
    isolEmRelRhoEA=cms.double(0.14),
    maxIsolHadRel03=cms.double(0.1),      #WP80
    isolHadRelRhoEA=cms.double(0.14),
    maxNrMissHits=cms.int32(1),
    maxDXY=cms.double(0.05)
)

heepBarrelCutsV40 = heepBarrelCutsV41.clone()
heepBarrelCutsV40.maxNrMissHits=cms.int32(0)
heepBarrelCutsV40.cuts =cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks:nrMissHits")

heepEndcapCutsV40 = heepEndcapCutsV41.clone()
heepEndcapCutsV40.maxNrMissHits=cms.int32(0)
heepEndcapCutsV40.cuts =cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolPtTrks:nrMissHits")

heepEndcapCutsV50 = heepEndcapCutsV41.clone()
heepEndcapCutsV50.hademConstTerm = 12.5
heepEndcapCutsV50.dEtaInConstTerm = 0.015
heepEndcapCutsV50.dEtaInGradTerm = 8.5E-5

heepBarrelCutsV50 = heepBarrelCutsV41.clone()
heepBarrelCutsV50.hademConstTerm = 2
heepBarrelCutsV50.dEtaInConstTerm = 0.016
heepBarrelCutsV50.dEtaInGradTerm = 1E-4

heepEndcapCutsV51 = heepEndcapCutsV50.clone()
heepEndcapCutsV51.maxDEtaIn=cms.double(0.006)
heepEndcapCutsV51.cuts=cms.string("et:detEta:ecalDriven:dEtaInSeed:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolPtTrks:nrMissHits:dxy")
heepEndcapCutsV51.minEta=cms.double(1.566)
heepEndcapCutsV51.maxEta=cms.double(2.5)

heepBarrelCutsV51 = heepBarrelCutsV50.clone()
heepBarrelCutsV51.maxDEtaIn=cms.double(0.004)
heepBarrelCutsV51.cuts=cms.string("et:detEta:ecalDriven:dEtaInSeed:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks:nrMissHits:dxy")
heepBarrelCutsV51.minEta=cms.double(0.)
heepBarrelCutsV51.maxEta=cms.double(1.4442)



heepBarrelCutsV60 = heepBarrelCutsV51.clone()
heepBarrelCutsV60.hademConstTerm = 1


heepEndcapCutsV60 = heepEndcapCutsV51.clone()
heepEndcapCutsV60.hademConstTerm = 5


heepBarrelCutsV6080XAOD = heepBarrelCutsV60.clone()
heepBarrelCutsV6080XAOD.maxNrSatCrysForShowerShapeCut = 0
heepBarrelCutsV6080XAOD.useValMapForTrkIso=True

heepEndcapCutsV6080XAOD = heepEndcapCutsV60.clone()
heepEndcapCutsV6080XAOD.maxNrSatCrysForShowerShapeCut = 0
heepEndcapCutsV6080XAOD.useValMapForTrkIso=True


heepBarrelCutsV61 = heepBarrelCutsV60.clone()
heepBarrelCutsV61.isolPtTrksRhoStart = 95.0
heepBarrelCutsV61.isolPtTrksRhoEA= 1.5

heepEndcapCutsV61 = heepEndcapCutsV60.clone()
heepEndcapCutsV61.isolPtTrksRhoStart = 100.0
heepEndcapCutsV61.isolPtTrksRhoEA= 0.5



heepBarrelCuts = heepBarrelCutsV6080XAOD.clone()
heepEndcapCuts = heepEndcapCutsV6080XAOD.clone()


heepBarrelCutsV31 = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks"),
    minEt=cms.double(25),
    minEta=cms.double(0.),
    maxEta=cms.double(1.442),
    maxDEtaIn=cms.double(0.005),
    dEtaInConstTerm =cms.double(0),
    dEtaInGradTerm =cms.double(0),
    maxDPhiIn=cms.double(0.09),
    maxHadem=cms.double(0.05),
    hademConstTerm=cms.double(0),
    maxSigmaIEtaIEta=cms.double(999.), #not defined for barrel
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
    maxNrMissHits=cms.int32(0),
    maxDXY=cms.double(0)
)

heepEndcapCutsV31 = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolHadDepth2:isolPtTrks"),
    minEt=cms.double(25),
    minEta=cms.double(1.56),
    maxEta=cms.double(2.5),
    maxDEtaIn=cms.double(0.007),
    dEtaInConstTerm =cms.double(0),
    dEtaInGradTerm =cms.double(0),
    maxDPhiIn=cms.double(0.09),
    maxHadem=cms.double(0.05),
    hademConstTerm=cms.double(0),
    maxSigmaIEtaIEta=cms.double(0.03),
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
    maxIsolPtTrksRel03=cms.double(0.09),  #WP80
    maxIsolEmRel03=cms.double(0.07),      #WP80
    maxIsolHadRel03=cms.double(0.1),      #WP80
    maxNrMissHits=cms.int32(0),
    maxDXY=cms.double(0)
)

heepBarrelCutsV31WithMissHitsCut =heepEndcapCutsV31.clone()
heepBarrelCutsV31WithMissHitsCut.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks:nrMissHits")
 
heepEndcapCutsV31WithMissHitsCut = heepEndcapCutsV31.clone()
heepEndcapCutsV31WithMissHitsCut.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolHadDepth2:isolPtTrks:nrMissHits")

