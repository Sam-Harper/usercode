# Import configurations
import FWCore.ParameterSet.Config as cms

def appendVIDModuleIdNames(id_module_name,id_names):

    idmod= __import__(id_module_name, globals(), locals(), ['idName','cutFlow'])
    for name in dir(idmod):
        item = getattr(idmod,name)
        if hasattr(item,'idName') and hasattr(item,'cutFlow'):
            id_names.append(item.idName.value())
    return id_names

def getVIDModulesIdNames(id_modules):
    id_names = cms.vstring()
    for id_mod in id_modules:
        appendVIDModuleIdNames(id_mod,id_names)
        
    print id_names
    return id_names

process = cms.Process("EGDEBUG")


import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles)
                            )                            



defaultEleIDModules =  [ 'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff',
                        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V1_cff',
                        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_noIso_V1_cff', 
                        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_iso_V1_cff',
                        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
                        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff',
                        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_HZZ_V1_cff',
                        ]
defaultPhoIDModules =  [ 'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Fall17_94X_V1_TrueVtx_cff',
                        'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Fall17_94X_V1_cff', 
                        'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Fall17_94X_V1p1_cff', 
                        'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff',
                        'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Spring16_nonTrig_V1_cff'
                        ]



import RecoEgamma.EgammaTools.calibratedPhotonProducerTRecoPhoton_cfi 
import RecoEgamma.EgammaTools.calibratedElectronProducerTRecoGsfElectron_cfi
process.eleDebugger = cms.EDAnalyzer("EGammaAODEleDebugger",
                                     src=cms.InputTag("gedGsfElectrons"),
                                     energyCalibProducer=cms.string("calibratedElectrons"),
                                     vidProducer=cms.string("egmGsfElectronIDs"),
                                     energyCalibNames = RecoEgamma.EgammaTools.calibratedElectronProducerTRecoGsfElectron_cfi.calibratedElectronProducerTRecoGsfElectron.valueMapsStored,
                                     vidNames = getVIDModulesIdNames(defaultEleIDModules)
                                     )
process.phoDebugger = cms.EDAnalyzer("EGammaAODPhoDebugger",
                                     src=cms.InputTag("gedPhotons"),
                                     energyCalibProducer=cms.string("calibratedPhotons"),
                                     vidProducer=cms.string("egmPhotonIDs"),
                                     energyCalibNames =  RecoEgamma.EgammaTools.calibratedPhotonProducerTRecoPhoton_cfi.calibratedPhotonProducerTRecoPhoton.valueMapsStored,
                                     vidNames = getVIDModulesIdNames(defaultPhoIDModules)

                                     )

process.p = cms.Path(process.eleDebugger*process.phoDebugger)

