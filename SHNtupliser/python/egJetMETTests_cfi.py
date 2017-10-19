import FWCore.ParameterSet.Config as cms

packedPFCandidates = cms.EDProducer("EGPackedPFCandCorrector",
                                    eles = cms.InputTag("slimmedElectronsReg"),
                                    phos = cms.InputTag("slimmedPhotonsReg"),
                                    oldPhos = cms.InputTag("slimmedPhotons",processName=cms.InputTag.skipCurrentProcess()),
                                    
                                    srcCands = cms.InputTag("packedPFCandidates",processName=cms.InputTag.skipCurrentProcess()),
                                    correctEles = cms.bool(True), 
                                    correctPhos = cms.bool(True), 
                                    useSCEnergy = cms.bool(True), 
                                    scEnergyEtThres = cms.double(300)
                                    
                                    )

mets = cms.EDProducer("PFMETProducer",
                      src = cms.InputTag("packedPFCandidates"),
                      alias = cms.string('pfMet'),
                      globalThreshold = cms.double(0.0),
                      calculateSignificance = cms.bool(False),
                      )

def customiseEGJetMET(process,tag,correctEles,correctPhos,useSCEnergy,scEnergyEtThres):
    
    from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
    
    setattr(process,'packedPFCandidates'+tag,packedPFCandidates.clone(correctEles=cms.bool(correctEles),correctPhos=cms.bool(correctPhos),useSCEnergy=cms.bool(useSCEnergy),scEnergyEtThres=cms.double(scEnergyEtThres)))
    setattr(process,"pfCHS"+tag,cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"+tag), cut = cms.string("fromPV")))

    setattr(process,"ak4PFJetsCHS"+tag,ak4PFJets.clone(src = 'pfCHS'+tag, doAreaFastjet = True))
    
    from SHarper.SHNtupliser.simplePatJetProducer_cfi import patJetsSimple,patJetCorrFactorsSimple
    setattr(process,'patJetCorrFactors'+tag,patJetCorrFactorsSimple.clone(src = cms.InputTag('ak4PFJetsCHS'+tag)))
    setattr(process,'slimmedJets'+tag,patJetsSimple.clone(jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactors"+tag)),jetSource = cms.InputTag('ak4PFJetsCHS'+tag)))
    
    setattr(process,'slimmedMETs'+tag,mets.clone(src = cms.InputTag('packedPFCandidates'+tag)))
    
                                  
    setattr(process,"jetMETSeq"+tag,cms.Sequence(getattr(process,'packedPFCandidates'+tag)*
                                                 getattr(process,'pfCHS'+tag)*
                                                 getattr(process,'ak4PFJetsCHS'+tag)*
                                                 getattr(process,'patJetCorrFactors'+tag)*
                                                 getattr(process,'slimmedJets'+tag)*
                                                 getattr(process,'slimmedMETs'+tag)
                                                 ))
                
    return getattr(process,'jetMETSeq'+tag)
