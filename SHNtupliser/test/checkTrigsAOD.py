def getFilterIndex(trigEvt,filterName):
    for index in range(0,trigEvt.sizeFilters()):
        if filterName==trigEvt.filterLabel(index): return index
    return trigEvt.sizeFilters()

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

# open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
events = Events("file:aod_1.root")

trigEvtHandle, trigEvtLabel = Handle("trigger::TriggerEvent"), ("hltTriggerSummaryAOD","","HLT")
filterName="hltDiEle33CaloIdLMWPMS2UnseededFilter"
for event in events:

    event.getByLabel(trigEvtLabel,trigEvtHandle)
    trigEvt = trigEvtHandle.product()
    filterIndex = getFilterIndex(trigEvt,filterName)
    for filterKey in trigEvt.filterKeys(filterIndex):
        obj = trigEvt.getObjects()[filterKey]
        
        print obj.pt(),obj.eta(),obj.phi()
