import FWCore.ParameterSet.Config as cms

#this is where the HEEP selection is defined

heepBarrelCutsV41 = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks:nrMissHits:dxy"),
    minEt=cms.double(35),
    minEta=cms.double(0.),
    maxEta=cms.double(1.442),
    maxDEtaIn=cms.double(0.005),
    maxDPhiIn=cms.double(0.06),
    maxHadem=cms.double(0.05),
    maxSigmaIEtaIEta=cms.double(999.), #not defined for barrel
    minE2x5Over5x5=cms.double(0.94),
    minE1x5Over5x5=cms.double(0.83),
    isolEmHadDepth1ConstTerm=cms.double(2), #cut is of the form <const + grad*(et-gradStart) where et-gradStart is 0 if negative
    isolEmHadDepth1GradTerm=cms.double(0.03),
    isolEmHadDepth1GradStart=cms.double(0.),
    isolHadDepth2ConstTerm=cms.double(999.), #not defined for barrel
    isolHadDepth2GradTerm=cms.double(0.0), #not defined for barrel
    isolHadDepth2GradStart=cms.double(0.), #not defined for barrel
    isolPtTrksConstTerm=cms.double(5.0),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    maxIsolPtTrksRel03=cms.double(0.09),  #WP80
    maxIsolEmRel03=cms.double(0.07),      #WP80
    maxIsolHadRel03=cms.double(0.1),      #WP80
    maxNrMissHits=cms.int32(0),
    maxDXY=cms.double(0.02)
)

heepEndcapCutsV41 = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolPtTrks:nrMissHits:dxy"),
    minEt=cms.double(35),
    minEta=cms.double(1.56),
    maxEta=cms.double(2.5),
    maxDEtaIn=cms.double(0.007),
    maxDPhiIn=cms.double(0.06),
    maxHadem=cms.double(0.05),
    maxSigmaIEtaIEta=cms.double(0.03),
    minE2x5Over5x5=cms.double(0.), #not defined for endcap
    minE1x5Over5x5=cms.double(0.), #not defined for endcap
    isolEmHadDepth1ConstTerm=cms.double(2.5),
    isolEmHadDepth1GradTerm=cms.double(0.03),
    isolEmHadDepth1GradStart=cms.double(50.),
    isolHadDepth2ConstTerm=cms.double(999), #no longer defined for endcap
    isolHadDepth2GradTerm=cms.double(0.0),#no longer defined for endcap
    isolHadDepth2GradStart=cms.double(0.),#no longer defined for endcap
    isolPtTrksConstTerm=cms.double(5.0),
    isolPtTrksGradTerm=cms.double(0.0),
    isolPtTrksGradStart=cms.double(0.),
    maxIsolPtTrksRel03=cms.double(0.09),  #WP80
    maxIsolEmRel03=cms.double(0.07),      #WP80
    maxIsolHadRel03=cms.double(0.1),      #WP80
    maxNrMissHits=cms.int32(0),
    maxDXY=cms.double(0.05)
)

heepBarrelCuts = heepBarrelCutsV41.clone()
heepEndcapCuts = heepEndcapCutsV41.clone()

heepBarrelCutsV40 = heepBarrelCutsV41.clone()
heepBarrelCutsV40.maxNrMissHits=cms.int32(0)
heepBarrelCutsV40.cuts =cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolPtTrks:nrMissHits")

heepEndcapCutsV40 = heepEndcapCutsV41.clone()
heepEndcapCutsV40.maxNrMissHits=cms.int32(0)
heepEndcapCutsV40.cuts =cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:isolEmHadDepth1:isolPtTrks:nrMissHits")



heepEffectiveAreas = cms.PSet (
    trackerBarrel = cms.double(0.),
    trackerEndcap = cms.double(0.),
    ecalBarrel = cms.double(0.14),
    ecalEndcap = cms.double(0.14),
    hcalBarrel = cms.double(0.14),
    hcalEndcap = cms.double(0.14)
              )                                

heepBarrelCutsV31 = cms.PSet (
    cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:isolEmHadDepth1:isolPtTrks"),
    minEt=cms.double(25),
    minEta=cms.double(0.),
    maxEta=cms.double(1.442),
    maxDEtaIn=cms.double(0.005),
    maxDPhiIn=cms.double(0.09),
    maxHadem=cms.double(0.05),
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
    maxDPhiIn=cms.double(0.09),
    maxHadem=cms.double(0.05),
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

