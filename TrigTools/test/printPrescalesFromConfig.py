

import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLT" )

import sys

#prescaleTableConfig="preScaleTable"
prescaleTableConfig=sys.argv[1]
prescaleColumn=int(sys.argv[2])
#print prescaleTableConfig,prescaleColumn

process.load(prescaleTableConfig)


if 'PrescaleService' not in process.__dict__:
    print "no prescales found"
else:
    print "prescales found: ",len(process.PrescaleService.prescaleTable)

    nrPSCol=len(process.PrescaleService.lvl1Labels)
    
    if prescaleColumn >= nrPSCol:
        print "prescale column ",prescaleColumn, " is greater than max prescale colmun ",nrPSCol-1
    else:
        if prescaleColumn<0:
            prescaleColumn=0
            for label in process.PrescaleService.lvl1Labels:
             #   print label,prescaleColumn
                prescaleColumn=prescaleColumn+1
                if label==process.PrescaleService.lvl1DefaultLabel:
                    break
            
    
        for pset in process.PrescaleService.prescaleTable: 
            pathName=pset.pathName.pythonValue()
            pathName=pathName.lstrip('\'')
            pathName=pathName.rstrip('\'')
   
            if pathName.startswith("HLT_"):
                print pathName, pset.prescales[prescaleColumn]

        print "prescale label: ",process.PrescaleService.lvl1Labels[prescaleColumn]
