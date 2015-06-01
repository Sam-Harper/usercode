
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis')
from SHarper.MCTools.mcCmdLineOptions_cfi import registerDefaultMCOptions
registerDefaultMCOptions(options)
options.register ('zPrimeModel',
                  "zPrimeSSM",
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "which Z' model to use")
options.register ('interferenceMode',
                  3,
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,          
                  "Z/gamma/Z' interference setting")
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


dV=0
dA=0
uV=0
uA=0
eV=0
eA=0
nuV=0
nuA=0
import math
if options.zPrimeModel=="zPrimeChi":
    print "Z' chi "
    sinThetaW=math.sqrt(0.23)
    dV=2*math.sqrt(6)*sinThetaW/3
    dA=-math.sqrt(6)*sinThetaW/3
    uV=0
    uA=math.sqrt(6)*sinThetaW/3
    eV=-dV
    eA=dA
    nuV=-math.sqrt(6)*sinThetaW/2;
    nuA=nuV

    
elif options.zPrimeModel=="zPrimeEta":
    print "Z' eta"
    sinThetaW=math.sqrt(0.23)
    dV=sinThetaW
    dA=sinThetaW/3
    uV=0
    uA=4*sinThetaW/3
    eV=-dV
    eA=dA
    nuV=-sinThetaW/3;
    nuA=nuV

elif options.zPrimeModel=="zPrimeI":
    print "Z' I"
    sinThetaW=math.sqrt(0.23)
    
    dV=math.sqrt(15)/3*sinThetaW
    dA=-math.sqrt(15)/3*sinThetaW
    uV=0
    uA=0
    eV=-dV
    eA=dA
    nuV=eV
    nuA=eA
    
elif options.zPrimeModel=="zPrimeSQ":
    print "Z' SQ"
    sinThetaW=math.sqrt(0.23)
    
    dV=3/2*sinThetaW
    dA=-7/6*sinThetaW
    uV=0
    uA=sinThetaW/3
    eV=-dV
    eA=dA
    nuV=-4/3*sinThetaW
    nuA=-4/3*sinThetaW

elif options.zPrimeModel=="zPrimeN":
    print "Z' N"
    sinThetaW=math.sqrt(0.23)
    
    dV=-math.sqrt(6)/6*sinThetaW
    dA=math.sqrt(6)/2*sinThetaW
    uV=0
    uA=math.sqrt(6)/3*sinThetaW
    eV=-dV
    eA=dA
    nuV=math.sqrt(6)/3*sinThetaW
    nuA=math.sqrt(6)/3*sinThetaW

elif options.zPrimeModel=="zPrimePSI":
    print "Z' PSI"
    dV=0
    dA=0.506809
    uV=0
    uA=0.506809
    eV=0
    eA=0.506809
    nuV=0.253405
    nuA=0.253405

elif options.zPrimeModel=="zPrimeSSM":
    print "Z' SSM"
    dV = -0.693
    dA = -1.
    uV = 0.387
    uA = 1.
    eV = -0.08
    eA = -1.
    nuV = 1
    nuA = 1
    
else:
    print options.zPrimeModel, " not recongised"


print dV,dA
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

                        'NewGaugeBoson:ffbar2gmZZPrime = on',
                        'Zprime:gmZmode = '+str(options.interferenceMode),
                        'Zprime:universality = on',
                        'Zprime:vd='+str(dV),
                        'Zprime:ad='+str(dA),
                        'Zprime:vu='+str(uV),
                        'Zprime:au='+str(uA),
                        'Zprime:ve='+str(eV),
                        'Zprime:ae='+str(eA),
                        'Zprime:vnue='+str(nuV),
                        'Zprime:anue='+str(nuA),
                        '32:m0 = '+str(options.mass),
                        '32:onMode = off',
                        '32:onIfAny = 11',
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
                                            datasetName=cms.string(options.zPrimeModel+"Inter"+str(options.interferenceMode)),
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

