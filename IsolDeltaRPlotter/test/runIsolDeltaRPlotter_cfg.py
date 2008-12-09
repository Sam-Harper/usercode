# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("USER")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkSummary = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet( limit = cms.untracked.int32(0)  ),
   
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )


# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("SHarper.IsolDeltaRPlotter.IsolDeltaRPlotter_cfi");

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('outputFast.root')
)


process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(''))

process.PoolSource.fileNames = ['file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_0.root',
                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_1.root',
                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_10.root',
                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_2.root',
                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_3.root',
                               'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_6.root',
                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_7.root',
                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_8.root',
                                'file:/media/usbdisk1/fastSimFiles/SingleElectronPt1000_cfi_GEN_FASTSIM_IDEAL_9.root']


process.p = cms.Path(process.isolDeltaRPlotter)
                    
