def rmTrigVersion(trigName):
    index=trigName.rfind("_v")
    if index!=-1:
        return trigName[:index+2]
    else:
        return trigName

def makeTrigNameDict(trigNames):
    trigNameDict={}
    for trigNr,trigName in enumerate(trigNames):
        trigNameDict[rmTrigVersion(trigName)]=trigNr
    return trigNameDict


# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
import ROOT
ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

#put your input files here
inputFiles=["file:miniAOD_1.root",]
            

events = Events(inputFiles)

#create the handle for the trigresults, if the MC is reHLT, change "HLT" to "HLT2"
trigBitsHandle, trigBitsLabel = Handle("edm::TriggerResults"), ("TriggerResults","","HLT")

#this is a dictionary with name of trig, nr pass, nr tot
trigData={"HLT_PFMET110_PFMHT110_IDTight_v" : [0,0],
          "HLT_MET200_v" : [0,0],
          "HLT_MET250_v" : [0,0],
          "HLT_MET300_v" : [0,0],
          "HLT_Photon175_v" : [0,0]}
          

for event in events:
    #get the trigger results from the event
    event.getByLabel(trigBitsLabel,trigBitsHandle)
    trigBits=trigBitsHandle.product()
    #so trigger names have a "_vX" version which changes, this makes
    #a dictionary of those names removed
    trigNames = makeTrigNameDict(event.object().triggerNames(trigBits).triggerNames())


    for trigName in trigData.keys():
        #this is in case the path isnt in the menu, it'll just ignore it
        try:
            if trigBits.accept(trigNames[trigName])==True:
                trigData[trigName][0]+=1
            trigData[trigName][1]+=1
        except KeyError:
            pass


        
print "trigger result"
for trigName in trigData.keys():
    nrPass = trigData[trigName][0]
    nrTot = trigData[trigName][1]
    print trigName,"nr pass:",nrPass,"/",nrTot,"eff:",float(nrPass)/nrTot
    
        

