
import FWCore.ParameterSet.Config as cms

def rmOutputMods(process):
    #print process.endpaths_()
    for endPath in process.endpaths_().keys():
        for modName in process.outputModules_().keys():
            outMod = getattr(process,modName)
            getattr(process,endPath).remove(outMod)



def rmAllEndPathsWithOutput(process):
    #print process.endpaths_()
    outputModuleNames=process.outputModules_().keys()
    for endPathName in process.endpaths_().keys():
        if len(endPathName)-endPathName.find("Output")==6 and len(endPathName)>=6:
            print "removing endpath ",endPathName
            delattr(process,endPathName)
#            rmPath(process,endPath)
            
        else:
            endPath = getattr(process,endPathName)
            pathModuleNames = endPath.moduleNames()
            for outModName in outputModuleNames:
                if outModName in pathModuleNames: 
                    print "removing endpath ",endPathName
                    delattr(process.endPathName)
#                    rmPath(process,endPath)
                    break
def rmPath(process,pathName):
     print "removing path ",pathName   
     delattr(process,pathName)     
     try:
         for psSet in process.PrescaleService.prescaleTable:
             if psSet.pathName.value()==pathName:
                 process.PrescaleService.prescaleTable.remove(psSet)
     except:
         pass
def rmPathPattern(process,pathPattern):
    for pathName in process.pathNames().split():
        if pathName.find(pathPattern)!=-1:
            rmPath(process,pathName)

           
    
def rmAllPathsExcept(process,pathsToKeep):
    for pathName in process.pathNames().split():
        if (pathName.find("HLT_")==0 or pathName.find("MC_")==0 or pathName.find("AlCa_")==0 or pathName.find("DST_")==0) and pathName not in pathsToKeep:
            rmPath(process,pathName)
            

def rmL1Seeds(process,l1SeedsToRM):
           
    for filterName in process.filterNames().split():
        filt=getattr(process,filterName)
        if filt.type_()=="HLTL1TSeed":# and filterName=="hltL1sDoubleEGor":
            #print filt.L1SeedsLogicalExpression.value()
            l1SeedsInExp=filt.L1SeedsLogicalExpression.value().split(" OR ")
            for l1SeedToRM in l1SeedsToRM:
                for l1SeedInExp in l1SeedsInExp:
                    if l1SeedInExp==l1SeedToRM: l1SeedsInExp.remove(l1SeedInExp)
            

            l1SeedStr=""
            for l1SeedInExp in l1SeedsInExp[:-1]:
                l1SeedStr+=l1SeedInExp+" OR "
          #  print filt.L1SeedsLogicalExpression.value(),filterName
            try:
                l1SeedStr+=l1SeedsInExp[-1]
            except:
                l1SeedStr+="L1_ZeroBias"
            filt.L1SeedsLogicalExpression=l1SeedStr    
            #print filt.L1SeedsLogicalExpression.value()

def setSaveTags(process,pathName,saveTagsValue):
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        filt = getattr(process,filterName)
        if type(filt).__name__=="EDFilter":
            if filt.hasParameter("saveTags"):
                param=filt.getParameter("saveTags")
                param.setValue(saveTagsValue)


def addOutputMod(process):
    
    process.hltOutputTot = cms.OutputModule( "PoolOutputModule",
                                             fileName = cms.untracked.string( "outputPhysicsEGammaCommissioning.root" ),
                                             fastCloning = cms.untracked.bool( False ),
                                             dataset = cms.untracked.PSet(
                                                 filterName = cms.untracked.string( "" ),
                                                 dataTier = cms.untracked.string( "RAW" )
                                             ),
                                             SelectEvents = cms.untracked.PSet(),
                                             outputCommands = cms.untracked.vstring( 'drop *',
                                                                                  #   'keep *',
                                                                                     'keep *_hltGtStage2ObjectMap_*_*',
                                                                                #     'keep FEDRawDataCollection_rawDataCollector_*_*',
                                                                                #     'keep FEDRawDataCollection_source_*_*',
                                                                                     'keep edmTriggerResults_*_*_*',
                                                                                     'keep triggerTriggerEvent_*_*_*',
                                                                                     'keep recoRecoEcalCandidates*_*_*_*',
                                                                                     'keep recoSuperClusters_*_*_*',
                                                                                     'keep recoCaloClusters_*_*_*',
                                                                                     'keep *_genParticles_*_*',
                                                                                     'keep *_addPileupInfo_*_*',
                                                                                     'keep *_externalLHEProducer_*_*',
                                                                                     'keep *_generator_*_*',
                                                                                     'keep *_hltEgammaGsfTracks*_*_*',
                                                                                     'keep recoElectronSeeds_*_*_*',
                                                                                     'keep *_nrEventsStorer*_*_*')

                                             )
    process.HLTOutput = cms.EndPath(process.hltOutputTot)
    process.HLTSchedule.insert(len(process.HLTSchedule),process.HLTOutput)
   

def outputCmdsSimpleSkim():
    res = cms.untracked.vstring('drop *',
                                'keep *_*_*_SIM',
                                "drop edmHepMCProduct_generatorSmeared__SIM",
                                "drop *_g4SimHits_*_*",
                                "keep *_addPileupInfo_*_*", 
                                "keep FEDRawDataCollection_rawDataCollector_*_HLTSkim",
                                #                                                          "keep *_mix_*AffectedAPVList_*",
                                'keep *_nrEventsStorer_*_*',
                                'keep *_nrEventsStorerPostPUFilter_*_*',
                                'keep *_hltTriggerSummaryAOD_*_HLTSkim',
                                'keep *_TriggerResults_*_HLTSkim',
                                
                                )
    return res

def outputCmdsRePixelLight():
    res = cms.untracked.vstring(
        "drop *",
        'keep recoRecoEcalCandidatesToValuefloatAssociation_*_*_*',
        "keep *_hltEgammaCandidates_*_*",
        "keep *_hltParticleFlowSuperClusterECALL1Seeded_*_*",
        "keep *_hltParticleFlowClusterECALL1Seeded_*_*",
        "keep *_hltParticleFlowClusterPSL1Seeded_*_*",
        "keep *_hltEgammaCandidatesUnseeded_*_*",
        "keep *_hltParticleFlowSuperClusterECALUnseeded_*_*",
        "keep *_hltParticleFlowClusterECALUnseeded_*_*",
        "keep *_hltParticleFlowClusterPSUnseeded_*_*",
        "keep *_hlt*Method2*_*_*",
        "keep *_hltTowerMaker*_*_*",
#        "keep *_hltSiPixelRecHits_*_*",
        "keep *_hltSiPixelClusters_*_*", 
        "keep *_hltSiPixelClustersCache_*_*", 
        "keep *_hltGtStage2Digis_*_*",
        "keep *_hltGtStage2ObjectMap_*_*",
        "keep *_hltScalersRawToDigi_*_*",
        "keep *_hltOnlineBeamSpot_*_*",
       # "keep *",
        )
    return res


def outputCmdsEgammaDebug():
    res = cms.untracked.vstring("drop *",
    'keep recoRecoEcalCandidatesToValuefloatAssociation_*_*_*',
    "keep *_hltEgammaCandidates_*_*",
    "keep *_hltParticleFlowSuperClusterECALL1Seeded_*_*",
    "keep *_hltParticleFlowClusterECALL1Seeded_*_*",
    "keep *_hltParticleFlowClusterPSL1Seeded_*_*",
    "keep *_hltEgammaCandidatesUnseeded_*_*",
    "keep *_hltParticleFlowSuperClusterECALUnseeded_*_*",
    "keep *_hltParticleFlowClusterECALUnseeded_*_*",
    "keep *_hltParticleFlowClusterPSUnseeded_*_*",
    "keep *_hltGtStage2Digis_*_*",
    "keep *_hltGtStage2ObjectMap_*_*",
    "keep *_hltScalersRawToDigi_*_*",
    #"keep *_hltOnlineBeamSpot_*_*",
    #"keep *_hltSiStripClusters_*_*",
    "keep *_addPileupInfo_*_*",
    "keep *_genParticles_*_*", 
    "keep *_externalLHEProducer_*_*",
    'keep *_hltTriggerSummaryAOD_*_*',
    'keep *_TriggerResults_*_*',
    )
    return res
def outputCmdsRePixelMedium():
    res = cms.untracked.vstring(
        "drop *",
        "keep *_hltParticleFlowSuperClusterECALL1Seeded_*_*",
        "keep *_hltParticleFlowClusterECALL1Seeded_*_*",
        "keep *_hltParticleFlowClusterPSL1Seeded_*_*",
        "keep *_hltParticleFlowSuperClusterECALUnseeded_*_*",
        "keep *_hltParticleFlowClusterECALUnseeded_*_*",
        "keep *_hltParticleFlowClusterPSUnseeded_*_*",
        "keep *_hlt*Method2*_*_*",
        "keep *_hltTowerMaker*_*_*",
        "keep *_hltFixedGridRhoFastjetAllCaloForMuons_*_*",
        "keep *_hltRechitInRegionsECAL_*_*",
#        "keep *_hltSiPixelRecHits_*_*",
        "keep *_hltSiPixelClusters_*_*", 
        "keep *_hltSiPixelClustersCache_*_*", 
        "keep *_hltSiStripRawToClustersFacility_*_*",
        "keep *_hltSiStripExcludedFEDListProducer_*_*",
        "keep *_hltGtStage2Digis_*_*",
        "keep *_hltGtStage2ObjectMap_*_*",
        "keep *_hltScalersRawToDigi_*_*",
        "keep *_hltOnlineBeamSpot_*_*",
        "keep *_hltSiStripClusters_*_*",
        "keep *_addPileupInfo_*_*",
        "keep *_genParticles_*_*", 
        "keep *_externalLHEProducer_*_*",
       # "keep *",
        )
    return res
def outputCmdsRePixelMediumWithGen():
    res = cms.untracked.vstring(
        "drop *",
        "keep *_hltParticleFlowSuperClusterECALL1Seeded_*_*",
        "keep *_hltParticleFlowClusterECALL1Seeded_*_*",
        "keep *_hltParticleFlowClusterPSL1Seeded_*_*",
        "keep *_hltParticleFlowSuperClusterECALUnseeded_*_*",
        "keep *_hltParticleFlowClusterECALUnseeded_*_*",
        "keep *_hltParticleFlowClusterPSUnseeded_*_*",
        "keep *_hlt*Method2*_*_*",
        "keep *_hltTowerMaker*_*_*",
        "keep *_hltFixedGridRhoFastjetAllCaloForMuons_*_*",
        "keep *_hltRechitInRegionsECAL_*_*",
#        "keep *_hltSiPixelRecHits_*_*",
        "keep *_hltSiPixelClusters_*_*", 
        "keep *_hltSiPixelClustersCache_*_*", 
        "keep *_hltSiStripRawToClustersFacility_*_*",
        "keep *_hltSiStripExcludedFEDListProducer_*_*",
        "keep *_hltGtStage2Digis_*_*",
        "keep *_hltGtStage2ObjectMap_*_*",
        "keep *_hltScalersRawToDigi_*_*",
        "keep *_hltOnlineBeamSpot_*_*",
        "keep *_addPileupInfo_*_*",
        "keep *_genParticles_*_*", 
        "keep *_externalLHEProducer_*_*",
       
        )
    return res
