
# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("HEEP")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = cms.string('MCRUN2_74_V9A')
#process.GlobalTag.globaltag = cms.string('GR_P_V56')
# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(50000),
    limit = cms.untracked.int32(10000000)
)

process.source = cms.Source("PoolSource",
                                #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                                #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
                             )


process.trigRateAna = cms.EDAnalyzer("TrigRateAna",
                                     datasetCode=cms.int32(0),                                     
                                     prevTrigResultsTag=cms.InputTag("TriggerResults","","HLTStd"),
                                     newTrigResultsTag=cms.InputTag("TriggerResults","","HLTNewGsf"),
                                     genEvtInfoTag=cms.InputTag("generator")
)

process.p = cms.Path(process.trigRateAna)

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing("analysis")
options.register ('inputFileList',"",
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  "list of input files")
options.parseArguments()

with open(options.inputFileList) as f:
    for line in f.readlines():
        line=line.replace("/pnfs/pp.rl.ac.uk/data/cms","");
        line=line.rstrip()
        print line
        process.source.fileNames.append(line)
#    from SHarper.SHNtupliser.addInputFiles import addInputFiles
#    addInputFiles(process.source,content)
      
from SHarper.SHNtupliser.datasetCodes import getDatasetCode
process.trigRateAna.datasetCode=getDatasetCode(process.source.fileNames[0])
#process.source.fileNames=cms.untracked.vstring("/store/user/sharper/747_HLT5/HLTReRun/GEN-SIM-RAW/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/AVE_20_BX_25ns_tsg_MCRUN2_74_V7-v1_GEN-SIM-RAW_HLTReRun/150801_215656/0000/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8_ntuples_AVE_20_BX_25ns_tsg_MCRUN2_74_V7-v1_HLTReRun_17.root")
