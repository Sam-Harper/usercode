import FWCore.ParameterSet.Config as cms

isolDeltaRPlotter = cms.EDAnalyzer("IsolDeltaRPlotter",
                                   electronsTag = cms.InputTag("pixelMatchGsfElectrons"),
                                   superClustersEBTag = cms.InputTag("correctedHybridSuperClusters"),
                                   superClustersEETag = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
                                   caloTowersTag = cms.InputTag('towerMaker'),
                                   nrBins = cms.int32(100), #nr hist bins
                                   xMin = cms.double(0.), #hist min
                                   xMax = cms.double(0.5), #hist max
                                   maxDeltaR = cms.double(1.), #max dR to fill hist with
                                   minEtaBarrel = cms.double(0.),
                                   maxEtaBarrel = cms.double(1.442),
                                   minEtaEndcap = cms.double(1.560),
                                   maxEtaEndcap = cms.double(2.5)
                                   )
 
