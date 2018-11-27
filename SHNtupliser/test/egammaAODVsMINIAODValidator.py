#!/usr/bin/env python

ele_vars = ["scPixCharge","isGsfCtfScPixChargeConsistent","isGsfScPixChargeConsistent","isGsfCtfChargeConsistent","isElectron","ctfGsfOverlap","ecalDrivenSeed","trackerDrivenSeed","shFracInnerHits","eSuperClusterOverP","eSeedClusterOverP","eSeedClusterOverPout","eEleClusterOverPout","deltaEtaSuperClusterTrackAtVtx","deltaEtaSeedClusterTrackAtCalo","deltaEtaEleClusterTrackAtCalo","deltaPhiSuperClusterTrackAtVtx","deltaPhiSeedClusterTrackAtCalo","deltaPhiEleClusterTrackAtCalo","deltaEtaSeedClusterTrackAtVtx","basicClustersSize","isEB","isEE","isGap","isEBEEGap","isEBGap","isEBEtaGap","isEBPhiGap","isEEGap","isEEDeeGap","isEERingGap","sigmaEtaEta","sigmaIetaIeta","sigmaIphiIphi","e1x5","e2x5Max","e5x5","r9","hcalDepth1OverEcal","hcalDepth2OverEcal","hcalOverEcal","hcalDepth1OverEcalBc","hcalDepth2OverEcalBc","hcalOverEcalBc","eLeft","eRight","eTop","eBottom","full5x5_sigmaEtaEta","full5x5_sigmaIetaIeta","full5x5_sigmaIphiIphi","full5x5_e1x5","full5x5_e2x5Max","full5x5_e5x5","full5x5_r9","full5x5_hcalDepth1OverEcal","full5x5_hcalDepth2OverEcal","full5x5_hcalOverEcal","full5x5_hcalDepth1OverEcalBc","full5x5_hcalDepth2OverEcalBc","full5x5_hcalOverEcalBc","full5x5_eLeft","full5x5_eRight","full5x5_eTop","full5x5_eBottom","scSigmaEtaEta","scSigmaIEtaIEta","scE1x5","scE2x5Max","scE5x5","hadronicOverEm","hadronicOverEm1","hadronicOverEm2","nSaturatedXtals","isSeedSaturated","dr03TkSumPt","dr03EcalRecHitSumEt","dr03HcalDepth1TowerSumEt","dr03HcalDepth2TowerSumEt","dr03HcalTowerSumEt","dr03HcalDepth1TowerSumEtBc","dr03HcalDepth2TowerSumEtBc","dr03HcalTowerSumEtBc","dr04TkSumPt","dr04EcalRecHitSumEt","dr04HcalDepth1TowerSumEt","dr04HcalDepth2TowerSumEt","dr04HcalTowerSumEt","dr04HcalDepth1TowerSumEtBc","dr04HcalDepth2TowerSumEtBc","dr04HcalTowerSumEtBc","convFlags","convDist","convDcot","convRadius","mva_Isolated","mva_e_pi","ecalDriven","passingCutBasedPreselection","passingPflowPreselection","ambiguous","passingMvaPreselection","trackFbrem","superClusterFbrem","numberOfBrems","fbrem","isEcalEnergyCorrected","correctedEcalEnergy","correctedEcalEnergyError","trackMomentumError","ecalEnergy","ecalEnergyError","caloEnergy","isEnergyScaleCorrected","pixelMatchSubdetector1","pixelMatchSubdetector2","pixelMatchDPhi1","pixelMatchDPhi2","pixelMatchDRz1","pixelMatchDRz2","et","eta","phi"]#"full5x5_e2x5Left","full5x5_e2x5Right","full5x5_e2x5Top","full5x5_e2x5Bottom"

def ele_diff(ele1,ele2):
    diff = False
    vars_to_check = ["ecalEnergy","ecalEnergyError","eta","phi"]
    vars_to_check = ele_vars
    
    for var in vars_to_check:
        if abs(getattr(ele1,var)()-getattr(ele2,var)())>0.0001:
            print "diff ele",var,getattr(ele1,var)(),getattr(ele2,var)()
            diff = True
    return diff

def pho_diff(pho1,pho2):
    diff = False
    vars_to_check = ["et","energy","eta","phi"]
    for var in vars_to_check:
        if abs(getattr(pho1,var)()-getattr(pho2,var)())>0.000001:
            print "diff pho",var,getattr(pho1,var)(),getattr(pho2,var)()
            diff=True
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


parser.add_argument('--aod',help='aod filename',required=True)
parser.add_argument('--miniaod',help='miniaod filename',required=True)
parser.add_argument('--prefix',help='file prefex',default="file:")
args = parser.parse_args()

file_prefix=args.prefix

miniaod_file = args.miniaod
aod_file = args.aod
#ref_file = "miniAOD-prodMC_PAT.root"
#ref_file = "/opt/ppd/scratch/harper/CMSSW/reco/CMSSW_942_EGReminiAODNewSS/src/miniAOD-prodData_PAT_full.root"
### Objects from file ###
eles_miniaod, ele_miniaod_label = Handle("std::vector<pat::Electron>"), "slimmedElectrons"
eles_aod, ele_aod_label = Handle("std::vector<reco::GsfElectron>"), "gedGsfElectrons"

phos_miniaod, pho_miniaod_label = Handle("std::vector<pat::Photon>"), "slimmedPhotons"
phos_aod, pho_aod_label = Handle("std::vector<reco::Photon"), "gedPhotons"


do_phos=True
do_eles=True

evtLUT = {}

events_miniaod = Events(miniaod_file)
for event_nr,event in enumerate(events_miniaod):
    runnr = event.eventAuxiliary().run()
    eventnr = event.eventAuxiliary().event()
    lumi = event.eventAuxiliary().luminosityBlock()
    if runnr not in evtLUT:
        evtLUT[runnr]={}
    if lumi not in evtLUT[runnr]:
        evtLUT[runnr][lumi]={}
    evtLUT[runnr][lumi][eventnr]=event_nr


events_aod = Events(aod_file)

nr_phos = 0
nr_diff_phos = 0

nr_eles = 0
nr_diff_eles = 0

min_pho_et  = 10
min_ele_et  = 5

for event_nr,event_aod in enumerate(events_aod):
    runnr = event_aod.eventAuxiliary().run()
    eventnr = event_aod.eventAuxiliary().event()
    lumi = event_aod.eventAuxiliary().luminosityBlock()
    event_found = events_miniaod.to(evtLUT[runnr][lumi][eventnr])
 
    event_id=str(event_aod.eventAuxiliary().run())+":"+str(event_aod.eventAuxiliary().luminosityBlock())+":"+str(event_aod.eventAuxiliary().event())

    if do_phos:
        event_aod.getByLabel(pho_aod_label,phos_aod)
        events_miniaod.getByLabel(pho_miniaod_label,phos_miniaod)
    
        for pho_nr,pho_aod in enumerate(phos_aod.product()):
            if pho_aod.et()<min_pho_et: continue
            pho_miniaod = match_by_sc(pho_aod,phos_miniaod.product())
            nr_phos+=1
            if pho_miniaod==None:
                print "pho ",pho_aod.et(),pho_aod.eta(),pho_aod.phi(),"not found"
                nr_diff_phos+=1
            else: 
 #               obj_comp(pho,pho_miniaod)
                if pho_diff(pho_aod,pho_miniaod):
                    nr_diff_phos+=1
            

    if do_eles:
        event_aod.getByLabel(ele_aod_label,eles_aod)
        events_miniaod.getByLabel(ele_miniaod_label,eles_miniaod) 
    
        for ele_nr,ele_aod in enumerate(eles_aod.product()):
            if ele_aod.et()<min_ele_et: continue
            nr_eles+=1
            ele_miniaod = match_by_sc(ele_aod,eles_miniaod.product())
            if ele_miniaod==None:
                print "ele ",ele_aod.et(),ele_aod.eta(),ele_aod.phi(),"not found"
                nr_diff_eles+=1
            else:
                #print "ele ",ele.et(),ele.eta(),ele.phi()," found"
                if ele_diff(ele_aod,ele_miniaod):
                    nr_diff_eles+=1
  
print "nrphos ",nr_phos," nr diff ",nr_diff_phos
print "nreles ",nr_eles," nr diff ",nr_diff_eles
