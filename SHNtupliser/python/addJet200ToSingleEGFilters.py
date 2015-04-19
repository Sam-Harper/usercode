import FWCore.ParameterSet.Config as cms
def addJet200ToSingleEGFilters(process):
    print "turning on EG Jet Seeding"
    for filterName in process.filterNames().split():
        if filterName.find("hltL1sL1SingleEG")==0:
            filt = getattr(process,filterName)
            if filt.hasParameter("L1SeedsLogicalExpression"):
                param=filt.getParameter("L1SeedsLogicalExpression")
                param.setValue(param.value()+" OR L1_SingleJet200")
            #    param.setValue("L1_SingleJet200")
                print "changed ",filterName, "to ",param.value()
           # else:
           #     print filterName,"has no save tags field"
  
    process.hltRechitInRegionsECAL = cms.EDProducer("HLTEcalRecHitInAllL1RegionsProducer",
                                                    recHitLabels = cms.VInputTag( 'hltEcalRecHit:EcalRecHitsEB','hltEcalRecHit:EcalRecHitsEE' ),
                                                    productLabels = cms.vstring( 'EcalRecHitsEB','EcalRecHitsEE' ),
                                                    l1InputRegions = cms.VPSet(
                                                    cms.PSet(
                                                        type=cms.string("L1EmParticle"),
                                                        inputColl=cms.InputTag( 'hltL1extraParticles','Isolated' ),
                                                        minEt=cms.double(5),
                                                        maxEt=cms.double(999),
                                                        regionEtaMargin=cms.double(0.14),
                                                        regionPhiMargin=cms.double(0.4)
                                                        ),
                                                    cms.PSet(
                                                        type=cms.string("L1EmParticle"),
                                                        inputColl=cms.InputTag( 'hltL1extraParticles','NonIsolated' ),
                                                        minEt=cms.double(5),
                                                        maxEt=cms.double(999),
                                                        regionEtaMargin=cms.double(0.14),
                                                        regionPhiMargin=cms.double(0.4)
                                                        ),                                                    
                                                    cms.PSet(
                                                        type=cms.string("L1JetParticle"),
                                                        inputColl=cms.InputTag( 'hltL1extraParticles','Central' ),
                                                        minEt=cms.double(200),
                                                        maxEt=cms.double(999),
                                                        regionEtaMargin=cms.double(0.14),
                                                        regionPhiMargin=cms.double(0.4)
                                                        ),
                                                        
                                                        )
                                                    )


    process.hltRechitInRegionsES = cms.EDProducer("HLTEcalRecHitInAllL1RegionsProducer",
                                                  recHitLabels = cms.VInputTag('hltEcalPreshowerRecHit:EcalRecHitsES'),
                                                  productLabels = cms.vstring( 'EcalRecHitsES' ),
                                                  l1InputRegions = cms.VPSet(
                                                  cms.PSet(
                                                      type=cms.string("L1EmParticle"),
                                                      inputColl=cms.InputTag( 'hltL1extraParticles','Isolated' ),
                                                      minEt=cms.double(5),
                                                      maxEt=cms.double(999),
                                                      regionEtaMargin=cms.double(0.14),
                                                      regionPhiMargin=cms.double(0.4)
                                                      ),
                                                  cms.PSet(
                                                      type=cms.string("L1EmParticle"),
                                                      inputColl=cms.InputTag( 'hltL1extraParticles','NonIsolated' ),
                                                      minEt=cms.double(5),
                                                      maxEt=cms.double(999),
                                                      regionEtaMargin=cms.double(0.14),
                                                      regionPhiMargin=cms.double(0.4)
                                                      ),
                                                  cms.PSet(
                                                        type=cms.string("L1JetParticle"),
                                                        inputColl=cms.InputTag( 'hltL1extraParticles','Central' ),
                                                        minEt=cms.double(200),
                                                        maxEt=cms.double(999),
                                                        regionEtaMargin=cms.double(0.14),
                                                        regionPhiMargin=cms.double(0.4)
                                                        ),
                                                  ),
                                                  )
