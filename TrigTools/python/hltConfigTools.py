
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
     for psSet in process.PrescaleService.prescaleTable:
         if psSet.pathName.value()==pathName:
             process.PrescaleService.prescaleTable.remove(psSet)

           
    
def rmAllPathsExcept(process,pathsToKeep):
    for pathName in process.pathNames().split():
        if (pathName.find("HLT_")==0 or pathName.find("MC_")==0 or pathName.find("AlCa_")==0 or pathName.find("DST_")==0) and pathName not in pathsToKeep:
            rmPath(process,pathName)
            
                


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
                                                                                     'keep *_nrEventsStorer_*_*')
                                             )
    process.HLTOutput = cms.EndPath(process.hltOutputTot)
    process.HLTSchedule.insert(len(process.HLTSchedule),process.HLTOutput)
   
