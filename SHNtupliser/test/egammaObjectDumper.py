#!/usr/bin/env python
import math
import json
def get_sc_seed_id(obj):
    if not obj.superCluster().isAvailable(): return None
    else: return obj.superCluster().seed().seed().rawId() 

def make_ele_dict(ele):
    _vars = ["et","eta","phi","pfIsolationVariables.sumChargedHadronPt","pfIsolationVariables.sumNeutralHadronEt","pfIsolationVariables.sumPhotonEt","pfIsolationVariables.sumChargedParticlePt","pfIsolationVariables.sumNeutralHadronEtHighThreshold","pfIsolationVariables.sumPhotonEtHighThreshold","pfIsolationVariables.sumPUPt","ecalPFClusterIso","hcalPFClusterIso"]
    ele_dict = {}
    ele_dict['seedId'] = get_sc_seed_id(ele)
    for var in _vars:
        if var.find(".")==-1:
            ele_dict[var] = getattr(ele,var)()
        else:
            var_split = var.split(".")
            if len(var_split)>2: print "warning var",var,"has more than two layers, only a single . is supported"
            ele_dict[var_split[0]+"_"+var_split[1]] = getattr(getattr(ele,var_split[0])(),var_split[1])
    return ele_dict

from DataFormats.FWLite import Events, Handle
import sys
import argparse
import ROOT

def main():
    oldargv = sys.argv[:]
    sys.argv = [ '-b-' ]
    ROOT.gROOT.SetBatch(True)
    sys.argv = oldargv
    ROOT.gSystem.Load("libFWCoreFWLite.so");
    ROOT.gSystem.Load("libDataFormatsFWLite.so");
    ROOT.FWLiteEnabler.enable()
    
    parser = argparse.ArgumentParser(description='compares E/gamma pat::Electrons/Photons')
    parser.add_argument('input',help='input filename')
    parser.add_argument('out',help='out filename')
    parser.add_argument('--min_et',help='min of objects to check',default=5.,type=float)
    args = parser.parse_args()
    
    eles_miniAOD, ele_label_miniAOD = Handle("std::vector<pat::Electron>"), "slimmedElectrons"
    phos_miniAOD, pho_label_miniAOD = Handle("std::vector<pat::Photon>"), "slimmedPhotons"
    

    events = Events(args.input)

    out_ele_data = {}
    out_pho_data = {}
       
    for event_nr,event in enumerate(events):
        runnr = event.eventAuxiliary().run()
        eventnr = event.eventAuxiliary().event()
        lumi = event.eventAuxiliary().luminosityBlock()
        
 
        event_id=str(event.eventAuxiliary().run())+":"+str(event.eventAuxiliary().luminosityBlock())+":"+str(event.eventAuxiliary().event())
        if event_id not in out_ele_data:
            out_ele_data[event_id]=[]
            out_pho_data[event_id]=[]
        else:
            print "event {} already added, skipping".format(event_id)
            continue

        event.getByLabel(ele_label_miniAOD,eles_miniAOD)
        event.getByLabel(pho_label_miniAOD,phos_miniAOD)
        
        for ele_nr,ele in enumerate(eles_miniAOD.product()):
            if ele.et()<args.min_et: continue
            
            out_ele_data[event_id].append(make_ele_dict(ele))

    with open(args.out,'w') as f:
        json.dump(out_ele_data,f)

if __name__=="__main__":
    main()
