import os
import FWCore.ParameterSet.Config as cms
import sys
process = cms.Process( "HLTDUMP" )

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['startup']

process.hltDumper =  cms.EDAnalyzer("DumpHLTMenuName",
                                    hltProcess=cms.string(sys.argv[3])
                                )
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)
 
import subprocess
fileNameRAW = subprocess.Popen(['dasgoclient','--query','file dataset='+sys.argv[2],'--limit','1','--format','json'],stdout=subprocess.PIPE).communicate()[0]
import json
fileNameJson=json.loads(fileNameRAW)
fileName=str(fileNameJson["data"][0]["file"][0]["name"])

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(fileName),
                            )

process.p = cms.Path(process.hltDumper) 
