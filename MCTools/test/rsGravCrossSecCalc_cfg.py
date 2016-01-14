# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: Configuration/GenProduction/ThirteenTeV/RSGravitonToEEMuMu_kMpl001_M-6000_13TeV_pythia8_cfi.py --fileout file:step1.root --mc --eventcontent RAWSIM --datatier GEN --conditions MCRUN2_71_V1::All --step GEN --python_filename step1_cfg.py --magField 38T_PostLS1 --beamspot NominalCollision2015 --customise SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1,Configuration/DataProcessing/Utils.addMonitoring --no_exec -n 30
import math



import FWCore.ParameterSet.VarParsing as VarParsing
import os

options = VarParsing.VarParsing ('analysis')
options.register ('minMass',
                  -1,
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.float,          
                  "min mass")
options.register ('maxMass',
                   -1, 
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.float,          
                  "max mass")

options.register ('mass',
                   1000, 
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.float,          
                  "mass")
options.register ('comEnergy',
                   13, 
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.float,          
                  "Centre of Mass Energy")
options.register ('kMpl',
                   0.1, 
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.float,          
                  "k/Mpl")
options.register ('outFile',
                   "output.root", 
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,          
                  "output filename (without tags)")

#options.parseArguments()
options.parseArguments()

print options.mass,options.minMass,options.maxMass

#print mass,minMass,maxMass,comEnergy,kMpl

import FWCore.ParameterSet.Config as cms

process = cms.Process('GEN')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedNominalCollision2015_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(

)
# Other statements
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_71_V1::All')

process.generator = cms.EDFilter("Pythia8GeneratorFilter",
        comEnergy = cms.double(options.comEnergy*1000),
        crossSection = cms.untracked.double(1e10),
        filterEfficiency = cms.untracked.double(1),
        maxEventsToPrint = cms.untracked.int32(0),
        pythiaHepMCVerbosity = cms.untracked.bool(False),
        pythiaPylistVerbosity = cms.untracked.int32(1),
        PythiaParameters = cms.PSet(
                processParameters = cms.vstring(
                        'Main:timesAllowErrors    = 10000',
                        'ParticleDecays:limitTau0 = on',
                        'ParticleDecays:tauMax = 10',
                        'Tune:pp 5',
                        'Tune:ee 3',
                        'PDF:pSet 13',
                        'ExtraDimensionsG*:all = on',
                        'ExtraDimensionsG*:kappaMG = '+str(5.4*options.kMpl),
                        '5100039:m0 = '+str(options.mass),
                        '5100039:onMode = off',
                        '5100039:onIfAny = 11',
                        'PhaseSpace:mHatMin = '+str(options.minMass),
                        'PhaseSpace:mHatMax = '+str(options.maxMass),

                ),
                parameterSets = cms.vstring('processParameters')
        )
)

process.ProductionFilterSequence = cms.Sequence(process.generator)

datasetCode=10001
process.crossSecTreeMaker =  cms.EDAnalyzer("CrossSecTreeMaker",
                                            mass=cms.double(options.mass),
                                            datasetName=cms.string("RSGrav"+str(options.kMpl).replace(".","p")),
                                            datasetCode=cms.int32(datasetCode),
                                            cmsEnergy=cms.double(options.comEnergy)
                                            )

process.pdfTreeMaker = cms.EDAnalyzer("PDFTreeMaker",
                                      datasetCode=cms.int32(datasetCode),
                                      genPartsTag=cms.InputTag("genParticles"),
                                      decayParticlePID = cms.int32(11),
                                      genEvtInfoTag = cms.InputTag("generator"),
                                      pdfWeightsTag=cms.InputTag("dummy"),
                                     #  pdfWeightsTag=cms.InputTag("pdfWeights:MRST2006nnlo")
                                      )


process.pdfWeights = cms.EDProducer("PdfWeightProducer",
                                    # Fix POWHEG if buggy (this PDF set will also appear on output,
                                    # so only two more PDF sets can be added in PdfSetNames if not "")
                                    #FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
                                    #GenTag = cms.untracked.InputTag("genParticles"),
                                    PdfInfoTag = cms.untracked.InputTag("generator"),
                                    PdfSetNames = cms.untracked.vstring(
                                        "cteq66.LHgrid",
                                    #  "MRST2006nnlo.LHgrid",
                                    #   "NNPDF10_100.LHgrid"
                                        )
      )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)


isCrabJob=False #script seds this if its a crab job
#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
    #process.crossSecTreeMaker.datasetCode = DATASETCODE

else:
    print "using user specified filename"
    
    process.TFileService.fileName=options.outFile
   # process.crossSecTreeMaker.datasetCode = int(sys.argv[len(sys.argv)-2])
    




# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen*process.crossSecTreeMaker*process.pdfTreeMaker)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)

# Schedule definition
process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.endjob_step)
# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.ProductionFilterSequence * getattr(process,path)._seq 

