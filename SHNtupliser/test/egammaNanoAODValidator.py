#!/usr/bin/env python
import math

def make_evt_lut_edm(events):
    evt_lut = {}
    for event_indx,event in enumerate(events):
        runnr = event.eventAuxiliary().run()
        eventnr = event.eventAuxiliary().event()
        lumi = event.eventAuxiliary().luminosityBlock()
        if runnr not in evt_lut:
            evt_lut[runnr]={}
        if lumi not in evt_lut[runnr]:
            evt_lut[runnr][lumi]={}
        evt_lut[runnr][lumi][eventnr]=event_indx
    return evt_lut

def make_evt_lut_tree(tree):
    evt_lut = {}
    for event_indx,event in enumerate(tree):
        runnr = event.run
        eventnr = event.event
        lumi = event.luminosityBlock
        if runnr not in evt_lut:
            evt_lut[runnr]={}
        if lumi not in evt_lut[runnr]:
            evt_lut[runnr][lumi]={}
        evt_lut[runnr][lumi][eventnr]=event_indx
    return evt_lut



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

    vars_to_check = ["ecalEnergy","ecalEnergyError","eta","phi","ecalDrivenSeed"]
    for var in vars_to_check:
        if abs(getattr(ele1,var)()-getattr(ele2,var)())>0.0001:
            print "diff ele {:.1f}, {:.2f}, {:.2f} :".format(ele1.et(),ele1.eta(),ele1.phi()),var,getattr(ele1,var)(),getattr(ele2,var)()
            diff = True


    return diff


class IDComp:
    def __init__(self,nano_name,family_name,working_points=[]):
        self.nano_name = nano_name
        self.family_name = family_name
        self.working_points = working_points

    def get_ele_nanoAOD(self,tree_nano,nano_ele_indx):
        return int(getattr(tree_nano,self.nano_name)[nano_ele_indx])

    def get_ele_miniAOD(self,ele):
        if self.working_points == []:
            return ele.electronID(self.family_name)
        else:
            for wp_nr,wp in enumerate(self.working_points):
                if ele.electronID("{}-{}".format(self.family_name,wp))==0:
                    return wp_nr #so first fail gives 0, 2nd fail gives 1 (as 1 passed)
            return len(self.working_points)

    def get_pho_nanoAOD(self,tree_nano,nano_ele_indx):
        return int(getattr(tree_nano,self.nano_name)[nano_ele_indx])

    def get_pho_miniAOD(self,pho):
        if self.working_points == []:
            return int(pho.photonID(self.family_name))
        else:
            for wp_nr,wp in enumerate(self.working_points):
                if pho.photonID("{}-{}".format(self.family_name,wp))==0:
                    return wp_nr #so first fail gives 0, 2nd fail gives 1 (as 1 passed)
            return len(self.working_points)


        
            

def get_ele_ids_to_check(era):

    ids = []
    if era=="2018_102X":
        ids = [IDComp("Electron_cutBased_Fall17_V1","cutBasedElectronID-Fall17-94X-V1",["veto","loose","medium","tight"]),
        
               IDComp("Electron_cutBased","cutBasedElectronID-Fall17-94X-V2",["veto","loose","medium","tight"]),
               IDComp("Electron_cutBased_HEEP","heepElectronID-HEEPV70"),
               IDComp("Electron_mvaFall17V1Iso_WP80","mvaEleID-Fall17-iso-V1-wp80"),
               IDComp("Electron_mvaFall17V1Iso_WP90","mvaEleID-Fall17-iso-V1-wp90"),
               IDComp("Electron_mvaFall17V1Iso_WPL","mvaEleID-Fall17-iso-V1-wpLoose"),
               IDComp("Electron_mvaFall17V1noIso_WP80","mvaEleID-Fall17-noIso-V1-wp80"),
               IDComp("Electron_mvaFall17V1noIso_WP90","mvaEleID-Fall17-noIso-V1-wp90"),
               IDComp("Electron_mvaFall17V1noIso_WPL","mvaEleID-Fall17-noIso-V1-wpLoose"),     
               IDComp("Electron_mvaFall17V2Iso_WP80","mvaEleID-Fall17-iso-V2-wp80"),
               IDComp("Electron_mvaFall17V2Iso_WP90","mvaEleID-Fall17-iso-V2-wp90"),
               IDComp("Electron_mvaFall17V2Iso_WPL","mvaEleID-Fall17-iso-V2-wpLoose"),
               IDComp("Electron_mvaFall17V2noIso_WP80","mvaEleID-Fall17-noIso-V2-wp80"),
               IDComp("Electron_mvaFall17V2noIso_WP90","mvaEleID-Fall17-noIso-V2-wp90"),
               IDComp("Electron_mvaFall17V2noIso_WPL","mvaEleID-Fall17-noIso-V2-wpLoose"),
               ]
    elif era=="2017_94X":
        ids = [#IDComp("Electron_cutBased","cutBasedElectronID-Fall17-94X-V1",["veto","loose","medium","tight"]),
               IDComp("Electron_cutBased_HEEP","heepElectronID-HEEPV70"),
               IDComp("Electron_mvaFall17V1Iso_WP80","mvaEleID-Fall17-iso-V1-wp80"),
               IDComp("Electron_mvaFall17V1Iso_WP90","mvaEleID-Fall17-iso-V1-wp90"),
               IDComp("Electron_mvaFall17V1Iso_WPL","mvaEleID-Fall17-iso-V1-wpLoose"),
               IDComp("Electron_mvaFall17V1noIso_WP80","mvaEleID-Fall17-noIso-V1-wp80"),
               IDComp("Electron_mvaFall17V1noIso_WP90","mvaEleID-Fall17-noIso-V1-wp90"),
               IDComp("Electron_mvaFall17V1noIso_WPL","mvaEleID-Fall17-noIso-V1-wpLoose"),     
               ]
    elif era=="2016_94X":
        ids = [IDComp("Electron_cutBased_Fall17_V1","cutBasedElectronID-Fall17-94X-V1",["veto","loose","medium","tight"]),
               IDComp("Electron_cutBased_Sum16","cutBasedElectronID-Summer16-80X-V1",["veto","loose","medium","tight"]),
               IDComp("Electron_cutBased_HEEP","heepElectronID-HEEPV70"),
               IDComp("Electron_mvaFall17V1Iso_WP80","mvaEleID-Fall17-iso-V1-wp80"),
               IDComp("Electron_mvaFall17V1Iso_WP90","mvaEleID-Fall17-iso-V1-wp90"),
               IDComp("Electron_mvaFall17V1Iso_WPL","mvaEleID-Fall17-iso-V1-wpLoose"),
               IDComp("Electron_mvaFall17V1noIso_WP80","mvaEleID-Fall17-noIso-V1-wp80"),
               IDComp("Electron_mvaFall17V1noIso_WP90","mvaEleID-Fall17-noIso-V1-wp90"),
               IDComp("Electron_mvaFall17V1noIso_WPL","mvaEleID-Fall17-noIso-V1-wpLoose"),     
               IDComp("Electron_mvaSpring16GP_WP80","mvaEleID-Spring16-GeneralPurpose-V1-wp80"),
               IDComp("Electron_mvaSpring16GP_WP90","mvaEleID-Spring16-GeneralPurpose-V1-wp90"),
               ]
    return ids

def get_pho_ids_to_check():
    return [#IDComp("Photon_cutBased","cutBasedPhotonID-Fall17-94X-V1",["veto","loose","medium","tight"]),

            #IDComp("Photon_cutBased","cutBasedPhotonID-Fall17-94X-V2",["veto","loose","medium","tight"]),
            #IDComp("Photon_mvaID_WP80","mvaPhoID-RunIIFall17-v1-wp80"),
            #IDComp("Photon_mvaID_WP90","mvaPhoID-RunIIFall17-v1-wp90"),
            ]


            

def comp_ele(ele,tree_nano,nano_ele_indx,era):
    ids_to_check = get_ele_ids_to_check(era)
    diff = False
    for ele_id in ids_to_check:
        nano_res = ele_id.get_ele_nanoAOD(tree_nano,nano_ele_indx)            
        mini_res = ele_id.get_ele_miniAOD(ele)

        
        if nano_res != mini_res:
            print "miss match ele {:.2f} {:.4f} id {} nano {} mini {}".format(ele.et(),ele.eta(),ele_id.nano_name,nano_res,mini_res)
            diff = True

    #now check variables
    userfloat_vars_to_check =[["mvaFall17V1Iso","ElectronMVAEstimatorRun2Fall17IsoV1Values"],
                              ["mvaFall17V1noIso","ElectronMVAEstimatorRun2Fall17NoIsoV1Values"],
                              ["mvaSpring16GP","ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values"]
                              ]

    for var_pair in userfloat_vars_to_check:
        var_nano = getattr(tree_nano,"Electron_"+var_pair[0])[nano_ele_indx]
        var_mini = ele.userFloat(var_pair[1])
        if(abs(var_nano-var_mini)>0.001):
            print "var miss match ele {:.2f} {:.2f} {:.2f} {} nano = {}, mini = {}, diff = {}".format(ele.et(),ele.eta(),ele.phi(),var_pair[0],var_nano,var_mini,var_nano-var_mini)
            diff = True

    #now check scale and smearing
    validateSS=True
    pt_nano = tree_nano.Electron_pt[nano_ele_indx]
    pt_mini = ele.pt()*ele.userFloat("ecalTrkEnergyPostCorr")/ele.energy()
    if(abs(pt_nano-pt_mini)>0.0001 and validateSS):
        print "pt miss match ele {:.2f} {:.2f} {:.2f} nano = {}, mini = {}, diff = {}".format(ele.et(),ele.eta(),ele.phi(),pt_nano,pt_mini,pt_nano-pt_mini)
        diff = True

    

    return diff

def comp_pho(pho,tree_nano,nano_pho_indx):
    ids_to_check = get_pho_ids_to_check()
    diff = False
    for pho_id in ids_to_check:
        nano_res = pho_id.get_pho_nanoAOD(tree_nano,nano_pho_indx)
        mini_res = pho_id.get_pho_miniAOD(pho)
        
        if nano_res != mini_res:
            print "miss match pho {:.2f} {:.4f} id {} nano {} mini {}".format(pho.et(),pho.eta(),pho_id.nano_name,nano_res,mini_res)
            diff = True

     #now check variables
    userfloat_vars_to_check =[#["mvaID","PhotonMVAEstimatorRunIIFall17v1Values"],
                              ]

    for var_pair in userfloat_vars_to_check:
        var_nano = getattr(tree_nano,"Photon_"+var_pair[0])[nano_pho_indx]
        var_mini = pho.userFloat(var_pair[1])
        if(abs(var_nano-var_mini)>0.01):
            print "var miss match pho {:.2f} {:.2f} {:.2f} {} nano = {}, mini = {}, diff = {}".format(pho.et(),pho.eta(),pho.phi(),var_pair[0],var_nano,var_mini,var_nano-var_mini)
            diff = True


      #now check scale and smearing
    validateSS=True
    pt_nano = tree_nano.Photon_pt[nano_pho_indx]
    pt_mini = pho.pt()*pho.userFloat("ecalEnergyPostCorr")/pho.energy()
    if(abs(pt_nano-pt_mini)>0.0001 and validateSS):
        print "pt miss match pho {:.2f} {:.2f} {:.2f} nano = {}, mini = {}, diff = {}".format(pho.et(),pho.eta(),pho.phi(),pt_nano,pt_mini,pt_nano-pt_mini)
        diff = True

    return diff


def match_by_sc(obj,objs_to_match):
    if not obj.superCluster().isAvailable(): return None
    
    for obj_to_match in objs_to_match:
        if obj_to_match.superCluster().isAvailable():
            if obj.superCluster().seed().seed().rawId()== obj_to_match.superCluster().seed().seed().rawId():
                return obj_to_match
    return None

def cal_delta_phi(phi1,phi2):
    dphi = phi1-phi2
    while dphi>=math.pi: 
        dphi -= 2*math.pi
    while dphi<=-math.pi:
        dphi += 2*math.pi
    return dphi

def cal_delta_r2(eta1,phi1,eta2,phi2):
    return (eta1-eta2)**2 + cal_delta_phi(phi1,phi2)**2

               
def get_nano_ele_indx(ele_eta,ele_phi,tree):
    max_dr2 = 0.01*0.01
    best_indx = -1
    for ele_indx in range(0,tree.nElectron):
        dr2 = cal_delta_r2(ele_eta,ele_phi,tree.Electron_eta[ele_indx],tree.Electron_phi[ele_indx])
        if dr2 < max_dr2:
            max_dr2 = dr2
            best_indx = ele_indx
    return best_indx

def get_nano_pho_indx(pho_eta,pho_phi,tree):
    max_dr2 = 0.01*0.01
    best_indx = -1
    for pho_indx in range(0,tree.nPhoton):
        dr2 = cal_delta_r2(pho_eta,pho_phi,tree.Photon_eta[pho_indx],tree.Photon_phi[pho_indx])
        if dr2 < max_dr2:
            max_dr2 = dr2
            best_indx = pho_indx
    return best_indx
        

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
    parser.add_argument('--miniAOD',help='input miniAOD filename',required=True)
    parser.add_argument('--nanoAOD',help='input nanoAOD filename',required=True)
    parser.add_argument('--min_et',help='min of objects to check',default=5.,type=float)
    parser.add_argument('--era',help='era of miniAOD',default="2017_94X",type=str)
    args = parser.parse_args()
    
    miniAOD_filename = args.miniAOD
    nanoAOD_filename = args.nanoAOD
    
### Objects from file ###
    eles_miniAOD, ele_label_miniAOD = Handle("std::vector<pat::Electron>"), "slimmedElectrons"
    phos_miniAOD, pho_label_miniAOD = Handle("std::vector<pat::Photon>"), "slimmedPhotons"
    


### Events loop ###
    events_miniAOD = Events(args.miniAOD)
    nanoAOD_file = ROOT.TFile(args.nanoAOD,"READ")
    tree_nanoAOD = ROOT.Events
   
    evt_lut_nanoAOD = make_evt_lut_tree(tree_nanoAOD)
   
    nr_eles = 0
    nr_diff_eles = 0
    nr_phos = 0
    nr_diff_phos = 0
    
    for event_nr,event in enumerate(events_miniAOD):
        runnr = event.eventAuxiliary().run()
        eventnr = event.eventAuxiliary().event()
        lumi = event.eventAuxiliary().luminosityBlock()
        
 
        event_id=str(event.eventAuxiliary().run())+":"+str(event.eventAuxiliary().luminosityBlock())+":"+str(event.eventAuxiliary().event())
        
        try:
            tree_index_nano = evt_lut_nanoAOD[runnr][lumi][eventnr]
        except KeyError:
            print "event {} not found, skipping".format(event_id)
            continue

        event.getByLabel(ele_label_miniAOD,eles_miniAOD)
        event.getByLabel(pho_label_miniAOD,phos_miniAOD)
        tree_nanoAOD.GetEntry(tree_index_nano)
        
        for ele_nr,ele in enumerate(eles_miniAOD.product()):
            if ele.et()<args.min_et: continue

            nr_eles+=1

            nano_ele_indx = get_nano_ele_indx(ele.eta(),ele.phi(),tree_nanoAOD)
            if nano_ele_indx < 0:
                print "ele {:.2f} {:.2f} {:.2f} not found in nano".format(ele.et(),ele.eta(),ele.phi())
            else:
                if comp_ele(ele,tree_nanoAOD,nano_ele_indx,args.era): nr_diff_eles+=1
        
        for pho_nr,pho in enumerate(phos_miniAOD.product()):
            if pho.et()<args.min_et: continue

            nr_phos+=1

            nano_pho_indx = get_nano_pho_indx(pho.eta(),pho.phi(),tree_nanoAOD)
            if nano_pho_indx < 0:
                print "pho {:.2f} {:.2f} {:.2f} not found in nano".format(pho.et(),pho.eta(),pho.phi())
            else:
                if comp_pho(pho,tree_nanoAOD,nano_pho_indx): nr_diff_phos+=1   
  
    print "nreles ",nr_eles," nr diff ",nr_diff_eles
    print "nrphos ",nr_phos," nr diff ",nr_diff_phos


if __name__=="__main__":
    main()
