# Import configurations
import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("PDF")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)




import sys
filePrefex="file:"
if(sys.argv[2].find("/pnfs/")==0):
    filePrefex="dcap://heplnx209.pp.rl.ac.uk:22125"

if(sys.argv[2].find("/store/")==0):
    filePrefex=""

process.source = cms.Source("PoolSource",
                 #         fileNames = cms.untracked.vstring(filePrefex+sys.argv[2]),
                       #     inputCommands = cms.untracked.vstring("drop *","keep *_source_*_*"),
                            fileNames = cms.untracked.vstring(),
)
for i in range(2,len(sys.argv)-2):
    print filePrefex+sys.argv[i]
    process.source.fileNames.extend([filePrefex+sys.argv[i],])
    
process.pdfTreeMaker = cms.EDAnalyzer("PDFTreeMaker",
                                      datasetCode=cms.int32(-1),
                                      genPartsTag=cms.InputTag("genParticles"),
                                      pdfWeightsTag=cms.InputTag("pdfWeights:cteq66")
                                      )
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root")
)


isCrabJob=False #script seds this if its a crab job

#if 1, its a crab job...
if isCrabJob:
    print "using crab specified filename"
    process.TFileService.fileName= "OUTPUTFILE"
    process.pdfTreeMaker.datasetCode = DATASETCODE

else:
    print "using user specified filename"
    
    process.TFileService.fileName= sys.argv[len(sys.argv)-1]
    process.pdfTreeMaker.datasetCode = int(sys.argv[len(sys.argv)-2])
    





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

process.p = cms.Path(
    process.pdfWeights*
    process.pdfTreeMaker)

