

def printEPValues(process,pathName):
    outputStr=pathName
    hasEPFilter=False
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        filt = getattr(process,filterName)
        if type(filt).__name__=="EDFilter":
            if filt.type_()=="HLTElectronOneOEMinusOneOPFilterRegional":
                print "old school tracking detected in %s" % pathName
                barrelCut = filt.getParameter("barrelCut").value()
                endcapCut = filt.getParameter("endcapCut").value()
                print "%s in %s : 1/E - 1/p cuts < %s (barrel) < %s (endcap) " % (filterName,pathName,barrelCut,endcapCut)
            
            if filt.type_()=="EgammaGenericFilter":
                if filt.getParameter("nonIsoTag").getModuleLabel()!="": print "mis configured E/gamma filter %s in %s" % (filterName,pathName)
                if filt.getParameter("isoTag").getProductInstanceLabel()=="OneOESuperMinusOneOP":
                    print "e/p filter detected"
                    barrelCut = filt.getParameter("thrRegularEB").value()
                    endcapCut = filt.getParameter("thrRegularEE").value()
                    print "%s in %s : 1/E - 1/p cuts < %s (barrel) < %s (endcap) " % (filterName,pathName,barrelCut,endcapCut)
                    
             
            
def printEPValuesAllPaths(process):
    for pathName in process.pathNames().split():
        printEPValues(process,pathName)



import argparse

parser = argparse.ArgumentParser(description='dumps the save tag filters of a menu')

parser.add_argument('hltMenuName',help="the python file containing the hlt menu, needs inputFiles=<name> (its due to cmssw var parsing, dont ask...)")
args = parser.parse_args()

hltMenuName=args.hltMenuName.rstrip(".py")
hltMenuName=hltMenuName.split("=")[1]
print hltMenuName
import importlib

mod = importlib.import_module(hltMenuName)
process = getattr(mod,"process")
#from setSaveTags import *


for pathName in process.pathNames().split():
    printEPValues(process,pathName)




#def isEgammaFilter(filterName):
#    if
        

