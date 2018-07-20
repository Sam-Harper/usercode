#!/usr/bin/env python


def make_ecalhit_dict(ecalhits):
    ecalhit_dict = {}
    for ecalhit in ecalhits:
        ecalhit_dict[ecalhit.detid().rawId()] = [ecalhit.energy(),ecalhit.checkFlag(8)]
    return ecalhit_dict

def comp_ecal_hits(ecalhits,ecalhits_ref):
    ecalhit_dict = make_ecalhit_dict(ecalhits)
    ecalhit_dict_ref = make_ecalhit_dict(ecalhits_ref)
    
    for detid in ecalhit_dict:
        if detid not in ecalhit_dict_ref:
            print "detid {} not in reference collection".format(detid)
        elif ecalhit_dict_ref[detid][0] != ecalhit_dict[detid][0]:
            print "detid {} differs in E {} vs {}".format(detid,ecalhit_dict_ref[detid][0],ecalhit_dict[detid][0])
        
    for detid in ecalhit_dict_ref:
        if detid not in ecalhit_dict:
            print "detid {} not in target collection".format(detid)

def sum_pfclus_energy(pfclusters):
    energy=0.
    for clus in pfclusters:
        energy += clus.energy()
    return energy

def comp_pfclus(pfclusters,pfclusters_ref):
    
    sum_clus = sum_pfclus_energy(pfclusters)
    sum_clus_ref = sum_pfclus_energy(pfclusters_ref)
    if sum_clus != sum_clus_ref:
        print " PF cluster energy sum diff  {} vs {} ".format(sum_clus,sum_clus_ref)

     
def comp_ecal_hits(ecalhits,ecalhits_ref):
    ecalhit_dict = make_ecalhit_dict(ecalhits)
    ecalhit_dict_ref = make_ecalhit_dict(ecalhits_ref)
    
    for detid in ecalhit_dict:
        if detid not in ecalhit_dict_ref:
            print "detid {} not in reference collection".format(detid)
        elif ecalhit_dict_ref[detid][0] != ecalhit_dict[detid][0]:
            print "detid {} differs in E {} vs {}".format(detid,ecalhit_dict_ref[detid][0],ecalhit_dict[detid][0])
        
    for detid in ecalhit_dict_ref:
        if detid not in ecalhit_dict:
            print "detid {} not in target collection".format(detid)
    



def match_by_sc(obj,objs_to_match):
    if not obj.superCluster().isAvailable(): return None
    
    for obj_to_match in objs_to_match:
        if obj_to_match.superCluster().isAvailable():
            if obj.superCluster().seed().seed().rawId()== obj_to_match.superCluster().seed().seed().rawId():
                return obj_to_match
    return None



import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
import ROOT
ROOT.gROOT.SetBatch(True)
sys.argv = oldargv
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()
from DataFormats.FWLite import Events, Handle

verbose=False


import argparse

parser = argparse.ArgumentParser(description='compares E/gamma pat::Electrons/Photons')


parser.add_argument('--ref',help='ref filename',required=True)
parser.add_argument('--target',help='target filename',required=True)
parser.add_argument('--prefix',help='file prefex',default="file:")
args = parser.parse_args()

file_prefix=args.prefix

target_file = args.target
ref_file = args.ref


ebrechits,ebrechits_label = Handle("edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > "),"ecalRecHit:EcalRecHitsEB"
ebrechits_ref = Handle("edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > ")

pfecalclusters,pfecalclusters_label = Handle("std::vector<reco::PFCluster>"),"particleFlowClusterECAL"
pfecalclusters_ref = Handle("std::vector<reco::PFCluster>")


### Events loop ###

evtLUT = {}

events_ref = Events(file_prefix+ref_file)
for event_nr,event in enumerate(events_ref):
    runnr = event.eventAuxiliary().run()
    eventnr = event.eventAuxiliary().event()
    lumi = event.eventAuxiliary().luminosityBlock()
    if runnr not in evtLUT:
        evtLUT[runnr]={}
    if lumi not in evtLUT[runnr]:
        evtLUT[runnr][lumi]={}
    evtLUT[runnr][lumi][eventnr]=event_nr




events = Events(file_prefix+target_file)


for event_nr,event in enumerate(events):
    runnr = event.eventAuxiliary().run()
    eventnr = event.eventAuxiliary().event()
    lumi = event.eventAuxiliary().luminosityBlock()
    event_found = events_ref.to(evtLUT[runnr][lumi][eventnr])
 
    event_id=str(event.eventAuxiliary().run())+":"+str(event.eventAuxiliary().luminosityBlock())+":"+str(event.eventAuxiliary().event())

    if not event_found:
        print "did not find event",event_id
        continue

    event.getByLabel(ebrechits_label,ebrechits)
    events_ref.getByLabel(ebrechits_label,ebrechits_ref)

 #   print "checking event ",event_id
    comp_ecal_hits(ebrechits.product(),ebrechits_ref.product())

    event.getByLabel(pfecalclusters_label,pfecalclusters)
    events_ref.getByLabel(pfecalclusters_label,pfecalclusters_ref)

    comp_pfclus(pfecalclusters.product(),pfecalclusters_ref.product())

  
