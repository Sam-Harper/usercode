#!/usr/bin/env python

def obj_comp(obj1,obj2):
    print "obj1 e/et/eta/phi",obj1.energy(),obj1.et(),obj1.eta(),obj1.phi()
    print "obj2 e/et/eta/phi",obj2.energy(),obj2.et(),obj2.eta(),obj2.phi()
    print "vars:"
    for name in obj1.userFloatNames():
        diff = abs(obj1.userFloat(name)-obj2.userFloat(name))>0.00001
        diff_str = ""
        if diff: diff_str="differs"
        print "     ",name,obj1.userFloat(name),obj2.userFloat(name),diff_str

def ele_diff(ele1,ele2):
    diff = False
    for name in ele1.userFloatNames():
 #       if name.find("Down")!=-1 or name.find("Up")!=-1: continue
#        if name=="energyEcalTrkErrPostCorr" or name=="energyEcalTrkPostCorr": continue
#        if name=="energySmearUp" or name =="energySmearDown": continue
        vars_to_skip = ["ecalTrkEnergyErrPostCorr","ecalTrkEnergyPostCorr","energyScaleDown","energyScaleGainDown","energyScaleGainUp","energyScaleStatDown","energyScaleStatUp","energyScaleSystDown","energyScaleSystUp","energyScaleUp","energySigmaDown","energySigmaPhiDown","energySigmaPhiUp","energySigmaRhoDown","energySigmaRhoUp","energySigmaUp"]
        if name in vars_to_skip: continue
        
        if name not in ele2.userFloatNames(): continue
        if abs(ele1.userFloat(name)-ele2.userFloat(name))>0.0001:
            print "diff ele {:.1f}, {:.2f}, {:.2f} :".format(ele1.et(),ele1.eta(),ele1.phi()),name,ele1.userFloat(name),ele2.userFloat(name)
            diff = True
    for name in ele1.userIntNames():
        
        if name not in ele2.userIntNames(): continue
        if ele1.userInt(name)!=ele2.userInt(name):
            print "diff ele {:.1f}, {:.2f}, {:.2f} :".format(ele1.et(),ele1.eta(),ele1.phi()),name,ele1.userInt(name),ele2.userInt(name)
            diff = True

    vars_to_check = ["ecalEnergy","ecalEnergyError","eta","phi"]
    for var in vars_to_check:
        if abs(getattr(ele1,var)()-getattr(ele2,var)())>0.0001:
            print "diff ele {:.1f}, {:.2f}, {:.2f} :".format(ele1.et(),ele1.eta(),ele1.phi()),var,getattr(ele1,var)(),getattr(ele2,var)()
            diff = True

    for ele_id in ele1.electronIDs():
        if ele2.isElectronIDAvailable(ele_id.first)==False: continue
        if ele_id.second != ele2.electronID(ele_id.first):
            print "diff ele {:.1f}, {:.2f}, {:.2f} : ID".format(ele1.et(),ele1.eta(),ele1.phi()),ele_id.first,ele_id.second,ele2.electronID(ele_id.first)

    return diff

def pho_diff(pho1,pho2):
    diff = False
    for name in pho1.userFloatNames():        
#        if name.find("energyScaleEt")==0: continue
        if name not in pho2.userFloatNames(): continue
        if abs(pho1.userFloat(name)-pho2.userFloat(name))>0.000001:
            print "diff pho {:.1f}, {:.2f}, {:.2f} :".format(pho1.et(),pho1.eta(),pho1.phi()),name,pho1.userFloat(name),pho2.userFloat(name)
            diff=True
    for name in pho1.userIntNames():        
#        if name.find("energyScaleEt")==0: continue
        if name not in pho2.userIntNames(): continue
        if pho1.userInt(name) != pho2.userInt(name):
            print "diff pho {:.1f}, {:.2f}, {:.2f} :".format(pho1.et(),pho1.eta(),pho1.phi()),name,pho1.userInt(name),pho2.userInt(name)
            diff=True

    vars_to_check = ["et","energy","eta","phi"]
    for var in vars_to_check:
        if abs(getattr(pho1,var)()-getattr(pho2,var)())>0.000001:
            print "diff pho {:.1f}, {:.2f}, {:.2f} :".format(pho1.et(),pho1.eta(),pho1.phi()),var,getattr(pho1,var)(),getattr(pho2,var)()
            diff=True

    for pho_id in pho1.photonIDs():
        if pho2.isPhotonIDAvailable(pho_id.first)==False: continue
        if pho_id.second != int(pho2.photonID(pho_id.first)):
            print "diff pho {:.1f}, {:.2f}, {:.2f} : ID ".format(pho1.et(),pho1.eta(),pho1.phi()),pho_id.first,pho_id.second,int(pho2.photonID(pho_id.first))

    if pho1.userInt("cutBasedPhotonID-Fall17-94X-V1-tight")!=pho2.userInt("cutBasedPhotonID-Fall17-94X-V1-tight"):
        print "diff pho {:.1f}, {:.2f}, {:.2f} : tigt id".format(pho1.et(),pho1.eta(),pho1.phi()),"cutBasedPhotonID-Fall17-94X-V1-tight",pho1.photonID("cutBasedPhotonID-Fall17-94X-V1-tight"),pho2.photonID("cutBasedPhotonID-Fall17-94X-V1-tight")

    return diff

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
#ref_file = "miniAOD-prodMC_PAT.root"
#ref_file = "/opt/ppd/scratch/harper/CMSSW/reco/CMSSW_942_EGReminiAODNewSS/src/miniAOD-prodData_PAT_full.root"
### Objects from file ###
eles, ele_label = Handle("std::vector<pat::Electron>"), "slimmedElectrons"
eles_ref = Handle("std::vector<pat::Electron>")
phos, pho_label = Handle("std::vector<pat::Photon>"), "slimmedPhotons"
phos_ref = Handle("std::vector<pat::Photon>")
### Events loop ###

min_pho_et = 10
min_ele_et = 5
do_phos=True
do_eles=True

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

nr_phos = 0
nr_diff_phos = 0

nr_eles = 0
nr_diff_eles = 0


for event_nr,event in enumerate(events):
    runnr = event.eventAuxiliary().run()
    eventnr = event.eventAuxiliary().event()
    lumi = event.eventAuxiliary().luminosityBlock()
    event_found = events_ref.to(evtLUT[runnr][lumi][eventnr])
 
    event_id=str(event.eventAuxiliary().run())+":"+str(event.eventAuxiliary().luminosityBlock())+":"+str(event.eventAuxiliary().event())

    if do_phos:
        event.getByLabel(pho_label,phos)
        events_ref.getByLabel(pho_label,phos_ref)
    
        for pho_nr,pho in enumerate(phos.product()):  
            if pho.et()<min_pho_et: continue
            pho_ref = match_by_sc(pho,phos_ref.product())
            nr_phos+=1
            if pho_ref==None:
                print "pho ",pho.et(),pho.eta(),pho.phi(),"not found"
                nr_diff_phos+=1
            else: 
 #               obj_comp(pho,pho_ref)
                if pho_diff(pho,pho_ref):
                    nr_diff_phos+=1
            

    if do_eles:
        event.getByLabel(ele_label,eles)
        events_ref.getByLabel(ele_label,eles_ref) 
    
        for ele_nr,ele in enumerate(eles.product()):
            if ele.et()<min_ele_et: continue
            nr_eles+=1
            ele_ref = match_by_sc(ele,eles_ref.product())
            if ele_ref==None:
                print "ele ",ele.et(),ele.eta(),ele.phi(),"not found"
                nr_diff_eles+=1
            else:
                #print "ele ",ele.et(),ele.eta(),ele.phi()," found"
                if ele_diff(ele,ele_ref):
                    nr_diff_eles+=1
  
print "nrphos ",nr_phos," nr diff ",nr_diff_phos
print "nreles ",nr_eles," nr diff ",nr_diff_eles
