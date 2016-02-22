
import FWCore.ParameterSet.Config as cms

def rmOutputMods(process):
    #print process.endpaths_()
    for endPath in process.endpaths_().keys():
        for modName in process.outputModules_().keys():
            outMod = getattr(process,modName)
            getattr(process,endPath).remove(outMod)

def rmPath(process,path):
    for moduleName in path.moduleNames():
        notAllCopiesRemoved=True
        while notAllCopiesRemoved:
            notAllCopiesRemoved = path.remove(getattr(process,moduleName))


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
        
    
def rmPaths(process,pathsToKeep):
    for pathName in process.pathNames().split():
        if (pathName.find("HLT_")==0 or pathName.find("MC_")==0 or pathName.find("AlCa_")==0 or pathName.find("DST_")==0) and pathName not in pathsToKeep:
            path = getattr(process,pathName)
            print "removing path ",pathName   
            delattr(process,pathName)
            #for moduleName in path.moduleNames():
            #    notAllCopiesRemoved=True
            #    while notAllCopiesRemoved:
            #        notAllCopiesRemoved = path.remove(getattr(process,moduleName))


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
                                             
                                             outputCommands = cms.untracked.vstring( 'drop *',
  #                                                                                  'keep *',
                                                                                     'keep *_hltL1GtObjectMap_*_*',
                                                                                     'keep FEDRawDataCollection_rawDataCollector_*_*',
                                                                                     'keep FEDRawDataCollection_source_*_*',
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
                                                                                     'keep recoElectronSeeds_*_*_*')
                                             )
    process.HLTOutput = cms.EndPath(process.hltOutputTot)
    process.HLTSchedule.insert(len(process.HLTSchedule),process.HLTOutput)
   
