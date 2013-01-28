import FWCore.ParameterSet.Config as cms



hltHybridSuperClustersActivity = cms.EDProducer( "HybridClusterProducer",
    eThreshA = cms.double( 0.003 ),
    basicclusterCollection = cms.string( "hybridBarrelBasicClusters" ),
    clustershapecollection = cms.string( "" ),
    ethresh = cms.double( 0.1 ),
    ewing = cms.double( 0.0 ),
    RecHitSeverityToBeExcluded = cms.vstring( 'kWeird' ),
    posCalcParameters = cms.PSet( 
      T0_barl = cms.double( 7.4 ),
      LogWeighted = cms.bool( True ),
      T0_endc = cms.double( 3.1 ),
      T0_endcPresh = cms.double( 1.2 ),
      W0 = cms.double( 4.2 ),
      X0 = cms.double( 0.89 )
    ),
    HybridBarrelSeedThr = cms.double( 1.0 ),
    dynamicPhiRoad = cms.bool( False ),
    shapeAssociation = cms.string( "hybridShapeAssoc" ),
    RecHitFlagToBeExcluded = cms.vstring(  ),
    useEtForXi = cms.bool( True ),
    step = cms.int32( 17 ),
    eThreshB = cms.double( 0.1 ),
    xi = cms.double( 0.0 ),
    eseed = cms.double( 0.35 ),
    ecalhitproducer = cms.string( "hltEcalRecHitAll" ),
    dynamicEThresh = cms.bool( False ),
    ecalhitcollection = cms.string( "EcalRecHitsEB" ),
    excludeFlagged = cms.bool( True ),
    superclusterCollection = cms.string( "" )
)

hltCorrectedHybridSuperClustersActivity = cms.EDProducer( "EgammaSCCorrectionMaker",
    corectedSuperClusterCollection = cms.string( "" ),
    sigmaElectronicNoise = cms.double( 0.15 ),
    superClusterAlgo = cms.string( "Hybrid" ),
    etThresh = cms.double( 5.0 ),
    rawSuperClusterProducer = cms.InputTag( "hltHybridSuperClustersActivity" ),
    applyEnergyCorrection = cms.bool( True ),
    isl_fCorrPset = cms.PSet(  ),
    VerbosityLevel = cms.string( "ERROR" ),
    recHitProducer = cms.InputTag( 'hltEcalRecHitAll','EcalRecHitsEB' ),
    fix_fCorrPset = cms.PSet(  ),
    modeEE = cms.int32( 0 ),
    modeEB = cms.int32( 0 ),
    dyn_fCorrPset = cms.PSet(  ),
    energyCorrectorName = cms.string( "EcalClusterEnergyCorrectionObjectSpecific" ),
    applyCrackCorrection = cms.bool( False ),
    hyb_fCorrPset = cms.PSet( 
      brLinearLowThr = cms.double( 1.1 ),
      fEtEtaVec = cms.vdouble( 0.0, 1.00121, -0.63672, 0.0, 0.0, 0.0, 0.5655, 6.457, 0.5081, 8.0, 1.023, -0.00181 ),
      brLinearHighThr = cms.double( 8.0 ),
      fBremVec = cms.vdouble( -0.04382, 0.1169, 0.9267, -9.413E-4, 1.419 )
    )
)

hltMulti5x5BasicClustersActivity = cms.EDProducer( "Multi5x5ClusterProducer",
    endcapHitCollection = cms.string( "EcalRecHitsEE" ),
    barrelClusterCollection = cms.string( "multi5x5BarrelBasicClusters" ),
    IslandEndcapSeedThr = cms.double( 0.18 ),
    doEndcap = cms.bool( True ),
    posCalcParameters = cms.PSet( 
      T0_barl = cms.double( 7.4 ),
      LogWeighted = cms.bool( True ),
      T0_endc = cms.double( 3.1 ),
      T0_endcPresh = cms.double( 1.2 ),
      W0 = cms.double( 4.2 ),
      X0 = cms.double( 0.89 )
    ),
    barrelShapeAssociation = cms.string( "multi5x5BarrelShapeAssoc" ),
    endcapShapeAssociation = cms.string( "multi5x5EndcapShapeAssoc" ),
    endcapHitProducer = cms.string( "hltEcalRecHitAll" ),
    clustershapecollectionEB = cms.string( "multi5x5BarrelShape" ),
    IslandBarrelSeedThr = cms.double( 0.5 ),
    barrelHitProducer = cms.string( "hltEcalRecHitAll" ),
    RecHitFlagToBeExcluded = cms.vstring(  ),
    barrelHitCollection = cms.string( "EcalRecHitsEB" ),
    clustershapecollectionEE = cms.string( "multi5x5EndcapShape" ),
    endcapClusterCollection = cms.string( "multi5x5EndcapBasicClusters" ),
    doBarrel = cms.bool( False )
)

hltMulti5x5SuperClustersActivity = cms.EDProducer( "Multi5x5SuperClusterProducer",
    barrelSuperclusterCollection = cms.string( "multi5x5BarrelSuperClusters" ),
    endcapEtaSearchRoad = cms.double( 0.14 ),
    barrelClusterCollection = cms.string( "multi5x5BarrelBasicClusters" ),
    dynamicPhiRoad = cms.bool( False ),
    endcapClusterProducer = cms.string( "hltMulti5x5BasicClustersActivity" ),
    barrelPhiSearchRoad = cms.double( 0.8 ),
    endcapPhiSearchRoad = cms.double( 0.6 ),
    barrelClusterProducer = cms.string( "hltMulti5x5BasicClustersActivity" ),
    seedTransverseEnergyThreshold = cms.double( 1.0 ),
    endcapSuperclusterCollection = cms.string( "multi5x5EndcapSuperClusters" ),
    barrelEtaSearchRoad = cms.double( 0.06 ),
    bremRecoveryPset = cms.PSet( 
      barrel = cms.PSet( 
        cryVec = cms.vint32( 16, 13, 11, 10, 9, 8, 7, 6, 5, 4, 3 ),
        cryMin = cms.int32( 2 ),
        etVec = cms.vdouble( 5.0, 10.0, 15.0, 20.0, 30.0, 40.0, 45.0, 55.0, 135.0, 195.0, 225.0 )
      ),
      endcap = cms.PSet( 
        a = cms.double( 47.85 ),
        c = cms.double( 0.1201 ),
        b = cms.double( 108.8 )
      )
    ),
    doEndcaps = cms.bool( True ),
    endcapClusterCollection = cms.string( "multi5x5EndcapBasicClusters" ),
    doBarrel = cms.bool( False )
)

hltMulti5x5SuperClustersWithPreshowerActivity = cms.EDProducer( "PreshowerClusterProducer",
    assocSClusterCollection = cms.string( "" ),
    preshStripEnergyCut = cms.double( 0.0 ),
    preshClusterCollectionY = cms.string( "preshowerYClusters" ),
    preshClusterCollectionX = cms.string( "preshowerXClusters" ),
    etThresh = cms.double( 0.0 ),
    preshRecHitProducer = cms.InputTag( 'hltESRecHitAll','EcalRecHitsES' ),
    endcapSClusterProducer = cms.InputTag( 'hltMulti5x5SuperClustersActivity','multi5x5EndcapSuperClusters' ),
    preshNclust = cms.int32( 4 ),
    preshClusterEnergyCut = cms.double( 0.0 ),
    preshSeededNstrip = cms.int32( 15 )
)

hltCorrectedMulti5x5SuperClustersWithPreshowerActivity = cms.EDProducer( "EgammaSCCorrectionMaker",
    corectedSuperClusterCollection = cms.string( "" ),
    sigmaElectronicNoise = cms.double( 0.15 ),
    superClusterAlgo = cms.string( "Multi5x5" ),
    etThresh = cms.double( 5.0 ),
    rawSuperClusterProducer = cms.InputTag( "hltMulti5x5SuperClustersWithPreshowerActivity" ),
    applyEnergyCorrection = cms.bool( True ),
    isl_fCorrPset = cms.PSet(  ),
    VerbosityLevel = cms.string( "ERROR" ),
    recHitProducer = cms.InputTag( 'hltEcalRecHitAll','EcalRecHitsEE' ),
    fix_fCorrPset = cms.PSet( 
      brLinearLowThr = cms.double( 0.9 ),
      fEtEtaVec = cms.vdouble( 1.0, -0.4386, -32.38, 0.6372, 15.67, -0.0928, -2.462, 1.138, 20.93 ),
      brLinearHighThr = cms.double( 6.0 ),
      fBremVec = cms.vdouble( -0.05228, 0.08738, 0.9508, 0.002677, 1.221 )
    ),
    modeEE = cms.int32( 0 ),
    modeEB = cms.int32( 0 ),
    dyn_fCorrPset = cms.PSet(  ),
    energyCorrectorName = cms.string( "EcalClusterEnergyCorrectionObjectSpecific" ),
    applyCrackCorrection = cms.bool( False ),
    hyb_fCorrPset = cms.PSet(  )
)


HLTEndcapClusterSequence = cms.Sequence(hltMulti5x5BasicClustersActivity*hltMulti5x5SuperClustersActivity*hltMulti5x5SuperClustersWithPreshowerActivity*hltCorrectedMulti5x5SuperClustersWithPreshowerActivity)
HLTBarrelClusterSequence = cms.Sequence(hltHybridSuperClustersActivity*hltCorrectedHybridSuperClustersActivity)
