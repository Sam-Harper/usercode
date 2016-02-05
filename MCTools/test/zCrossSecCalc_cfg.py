
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
from SHarper.MCTools.mcCmdLineOptions_cfi import registerDefaultMCOptions
registerDefaultMCOptions(options)
options.parseArguments()


import FWCore.ParameterSet.Config as cms

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
                    #     'PDF:pSet 13',

                        'WeakSingleBoson:ffbar2gmZ = on',
                        '23:onMode = off',
                        '23:onIfAny = 11',
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
                                            datasetName=cms.string("ZBoson"),
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

