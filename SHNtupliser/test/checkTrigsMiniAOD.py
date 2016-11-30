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
events = Events("file:miniAOD_1.root")

trigObjs = Handle("std::vector<pat::TriggerObjectStandAlone>")

for event in events:

    event.getByLabel("selectedPatTrigger",trigObjs)
    for obj in trigObjs.product():
        
        filters = ""
        for filt in obj.filterLabels():
            filters+=" "+filt

        print obj.pt(),obj.eta(),obj.phi(),filters
