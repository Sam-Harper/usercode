# dumps the ECAL crystal geometry to json for use outside CMSSW
# runs on an empty source, the run number just sets the conditions IOV
#
# usage: cmsRun dumpEcalGeomJson.py globalTag=auto:run3_data_prompt firstRun=402000 outFilename=ebGeom.json

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('globalTag', 'auto:run3_data_prompt',
                 VarParsing.multiplicity.singleton, VarParsing.varType.string,
                 "global tag to use for the geometry conditions")
options.register('firstRun', 402000,
                 VarParsing.multiplicity.singleton, VarParsing.varType.int,
                 "run number used to select the conditions IOV")
options.register('outFilename', 'ebGeom.json',
                 VarParsing.multiplicity.singleton, VarParsing.varType.string,
                 "output json filename (endcap goes to <name>.ee.json if enabled)")
options.register('dumpEndcap', False,
                 VarParsing.multiplicity.singleton, VarParsing.varType.bool,
                 "also dump the endcap geometry")
options.parseArguments()

process = cms.Process("GEOMDUMP")

process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, options.globalTag, '')

# castor has no geometry payload in run3 data GTs so restrict the builder to what we need
process.CaloGeometryBuilder.SelectedCalos = cms.vstring("EcalBarrel", "EcalEndcap")

process.source = cms.Source("EmptySource",
                            firstRun=cms.untracked.uint32(options.firstRun))
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1))

process.dumpEcalGeomJson = cms.EDAnalyzer("DumpEcalGeomJson",
                                          outputFilename=cms.string(options.outFilename),
                                          globalTag=cms.string(process.GlobalTag.globaltag.value()),
                                          dumpEndcap=cms.bool(bool(options.dumpEndcap)))

process.p = cms.Path(process.dumpEcalGeomJson)

print(f"dumping ecal geometry with GT {process.GlobalTag.globaltag.value()} run {options.firstRun} to {options.outFilename}")
