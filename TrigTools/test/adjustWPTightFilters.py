import FWCore.ParameterSet.Config as cms

def printNonWPFilters(process,pathName):
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        if filterName.find("WPTight")==-1: continue
        filt = getattr(process,filterName)
        if type(filt).__name__=="EDFilter" and filterName.find("WPTight")==-1:
            print "path",pathName," has non WPTight filter ",filterName,filt.type_()


def getWPTightFilters(process,pathName,filters,varProdName,varInstName):
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        if filterName.find("WPTight")==-1: continue
        filt = getattr(process,filterName)
     #   if type(filt).__name__=="EDFilter" and filt.type_()=="HLTEgammaGenericQuadraticEtaFilter":
     #       print filterName,filt.getParameter("varTag").getModuleLabel().find(varProdName),filt.getParameter("varTag").getProductInstanceLabel(),varInstName
        if type(filt).__name__=="EDFilter" and filt.type_()=="HLTEgammaGenericQuadraticEtaFilter" and filt.getParameter("varTag").getModuleLabel().find(varProdName)==0 and  filt.getParameter("varTag").getProductInstanceLabel()==varInstName:
            filters.add(filterName)
    
def modifyECALIsoFilter(filt):
    filt.effectiveAreas = cms.vdouble( 0.2,0.2,0.25,0.3)
    filt.thrOverEEB1 = cms.vdouble( 0.03 )
    filt.thrOverEEB2 = cms.vdouble( 0.03 )
    filt.thrOverEEE1 = cms.vdouble( 0.025 )
    filt.thrOverEEE2 = cms.vdouble( 0.025 )
    filt.thrRegularEB1 = cms.vdouble( 1.75 )
    filt.thrRegularEB2 = cms.vdouble( 1.75 )
    filt.thrRegularEE1 = cms.vdouble( 1.0 )
    filt.thrRegularEE2 = cms.vdouble( 2.0 )

def modifyHCALIsoFilter(filt):
    filt.effectiveAreas = cms.vdouble( 0.2,0.2,0.4,0.5)
    filt.thrOverEEB1 = cms.vdouble( 0.03 )
    filt.thrOverEEB2 = cms.vdouble( 0.03 )
    filt.thrOverEEE1 = cms.vdouble( 0.03 )
    filt.thrOverEEE2 = cms.vdouble( 0.03 )
    filt.thrRegularEB1 = cms.vdouble( 2.5 )
    filt.thrRegularEB2 = cms.vdouble( 3.0 )
    filt.thrRegularEE1 = cms.vdouble( 1.0 )
    filt.thrRegularEE2 = cms.vdouble( 2.0 ) 
    filt.etaBoundaryEE12 = cms.double( 2.0 )
    filt.absEtaLowEdges = cms.vdouble( 0.0, 1.0, 1.479, 2.0 )

def modifyHEFilter(filt):
    filt.effectiveAreas = cms.vdouble( 0.1,0.1,0.3,0.5)
    filt.thrOverEEB1 = cms.vdouble( 0.03 )
    filt.thrOverEEB2 = cms.vdouble( 0.03 )
    filt.thrOverEEE1 = cms.vdouble( 0.03 )
    filt.thrOverEEE2 = cms.vdouble( 0.03)
    filt.thrRegularEB1 = cms.vdouble( 0.75 )
    filt.thrRegularEB2 = cms.vdouble( 2.25 )
    filt.thrRegularEE1 = cms.vdouble( 2.0 )
    filt.thrRegularEE2 = cms.vdouble( 4.0 )


import argparse

parser = argparse.ArgumentParser(description='dumps the save tag filters of a menu')

parser.add_argument('hltMenuName',help="the python file containing the hlt menu, needs inputFiles=<name> (its due to cmssw var parsing, dont ask...)")
args = parser.parse_args()

hltMenuName=args.hltMenuName.rstrip(".py")
hltMenuName=hltMenuName.split("=")[1]
#print hltMenuName
import importlib

mod = importlib.import_module(hltMenuName)
process = getattr(mod,"process")

varProdNames=["hltEgammaHoverE","hltEgammaEcalPFClusterIso","hltEgammaHcalPFClusterIso"]
varInstName=""

filters = set()

for pathName in process.pathNames().split():
    for varProdName in varProdNames:
        getWPTightFilters(process,pathName,filters,varProdName,varInstName)


for pathName in process.pathNames().split():
    if pathName.find("WPTight")!=0:
        printNonWPFilters(process,pathName)

for filterName in filters:
    filt = getattr(process,filterName) 
    if filt.getParameter("varTag").getModuleLabel()=="hltEgammaHoverE":
        modifyHEFilter(filt)
    if filt.getParameter("varTag").getModuleLabel()=="hltEgammaEcalPFClusterIso":
        modifyECALIsoFilter(filt)
    if filt.getParameter("varTag").getModuleLabel()=="hltEgammaHcalPFClusterIso":
        modifyHCALIsoFilter(filt)
    

print "import FWCore.ParameterSet.Config as cms"
print 'process = cms.Process( "HLTX" )'
seqStr = "process.HLTEgammaWPTightFilters = cms.Sequence("

for filterName in filters:
    seqStr+="process."+filterName+" * "
    
    filt = getattr(process,filterName)
    print "process."+filterName+" = "+filt.dumpPython()

print seqStr.rstrip("* ")+")"
print 'process.hltBoolFalse = cms.EDFilter( "HLTBool",result = cms.bool( False ))'
print "process.HLT_DummyWPTightPath_v1 = cms.Path(process.hltBoolFalse + process.HLTEgammaWPTightFilters)"

