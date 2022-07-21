#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/EgammaObject.h"
#include "L1Trigger/L1TGlobal/plugins/L1TGlobalProducer.h"
#include "HLTrigger/Egamma/plugins/HLTEgammaL1TMatchFilterRegional.cc"
#include "CondFormats/DataRecord/interface/L1TGlobalParametersRcd.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include <vector>
#include <string>
#include <iostream>
#include <TH1F.h>
#include <TH2D.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include "Math/VectorUtil.h"
#include <stdlib.h>

#define TWOPI 6.283185308

/****************************************************************************
 Simple example on how to access the L1 decision via  l1t::L1TGlobalUtil
 To make things easier it uses HLTPrescaleProvider to obtain the  
 l1t::L1TGlobalUtil object

 note, its very important that you run with the correct global tag as it
 retrieves the menu from this

 

 author Sam  Harper (RAL), 2019
*****************************************************************************/

class EfficiencyCalculator : public edm::EDAnalyzer {
 
private:
  
  HLTPrescaleProvider hltPSProv_;  
  std::string hltProcess_; //name of HLT process, usually "HLT"
  edm::EDGetTokenT<std::vector<trigger::EgammaObject> > eleToken_;
  edm::EDGetTokenT<edm::TriggerResults > hltToken_;
  edm::EDGetTokenT<trigger::TriggerEvent > hltsevtToken_;
  edm::EDGetTokenT<std::vector<reco::GenParticle> > genToken_;
  edm::EDGetTokenT<trigger::TriggerFilterObjectWithRefs > L1Token_;
  edm::Service<TFileService> fs;
  double endcap_end_ = 2.65;
  TH1D* num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EB;
  TH1D* num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EE;

  TH1D* num_ele_pt_passL1seeds_EB;
  TH1D* num_ele_pt_passL1seeds_EE;

  TH1D* den_ele_pt_EB;
  TH1D* den_ele_pt_EE;

public:
  explicit EfficiencyCalculator(const edm::ParameterSet& iConfig);
  ~EfficiencyCalculator(){}
  
 private:
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob(){}
};


EfficiencyCalculator::EfficiencyCalculator(const edm::ParameterSet& iConfig):
  hltPSProv_(iConfig,consumesCollector(),*this), //it needs a referernce to the calling module for some reason, hence the *this
  hltProcess_("HLTX")
{
  eleToken_     = consumes<std::vector<trigger::EgammaObject> >(edm::InputTag("hltEgammaHLTExtra"));
  hltToken_     = consumes<edm::TriggerResults>(edm::InputTag("TriggerResults","","HLTX"));
  hltsevtToken_ = consumes<trigger::TriggerEvent>(edm::InputTag("hltTriggerSummaryAOD","","HLTX"));
  genToken_     = consumes<std::vector<reco::GenParticle> >(edm::InputTag("genParticles"));
  L1Token_      = consumes<trigger::TriggerFilterObjectWithRefs>(edm::InputTag("hltL1sSingleEGor"));


  num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EB = fs->make<TH1D>("num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EB",";pT;nEntries",100,0,500);
  num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EE = fs->make<TH1D>("num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EE",";pT;nEntries",100,0,500);

  num_ele_pt_passL1seeds_EB = fs->make<TH1D>("num_ele_pt_passL1seeds_EB",";pT;nEntries",100,0,500);
  num_ele_pt_passL1seeds_EE = fs->make<TH1D>("num_ele_pt_passL1seeds_EE",";pT;nEntries",100,0,500);

  den_ele_pt_EB = fs->make<TH1D>("den_ele_pt_EB",";pT;nEntries",100,0,500);
  den_ele_pt_EE = fs->make<TH1D>("den_ele_pt_EE",";pT;nEntries",100,0,500);

}


const int getFilterIndex(const trigger::TriggerEvent& trigEvt, const std::string filterName){
  for(auto i=0; i < trigEvt.sizeFilters(); i++){
    if(filterName==trigEvt.filterLabel(i)) return i;
  }
  return trigEvt.sizeFilters();  
}

std::vector<const trigger::TriggerObject*> getListFilterPassedObj(const std::string filterName,const trigger::TriggerEvent& hlts){
  std::vector<const trigger::TriggerObject*> eg_trig_objs;
  const int filterIndex = getFilterIndex(hlts,filterName);
  if(filterIndex < hlts.sizeFilters() ){
    for(auto filterKey : hlts.filterKeys(filterIndex)){
      auto& obj = hlts.getObjects()[filterKey];
      eg_trig_objs.push_back(&obj);
    }
  }
  return eg_trig_objs;
}


std::vector<const trigger::TriggerObject*> matchTrigObjs(const float eta,const float phi,std::vector<const trigger::TriggerObject*> trigObjs,const float maxDeltaR=0.1)
{
  std::vector<const trigger::TriggerObject*> matchedObjs;
  const float maxDR2 = maxDeltaR*maxDeltaR;
  for(auto& trigObj : trigObjs){
    const float deltaR2 = reco::deltaR2(eta, phi, trigObj->eta(), trigObj->phi());
    if(deltaR2<maxDR2) matchedObjs.push_back(trigObj);
  }
  return matchedObjs;
}

std::vector<const reco::GenParticle*> getGenparts(const std::vector<reco::GenParticle>& genparts,const int pid=11, bool antipart=true, const int status=1){

  std::vector<const reco::GenParticle*> selected;
  if(genparts.empty()) return selected;

  for(auto& part : genparts){
    const int pdg_id = part.pdgId();
    if(pdg_id == pid || (antipart && abs(pdg_id) == abs(pid))){
      if(part.isHardProcess() && status == 1){
	selected.push_back(&part);
      }
    }
  }
  return selected;
}

float matchToGen(const float eta,const float phi,const std::vector<reco::GenParticle>& genparts,const int pid=11,bool antipart=true,const float max_dr=0.1,const int status=1){

  float best_dr2 = max_dr*max_dr;
  float best_pt = -1;
  auto selected_parts = getGenparts(genparts,pid,antipart,status);
  for(auto part : selected_parts){
    const float dr2 = reco::deltaR2(eta, phi, part->eta(), part->phi());
    if(dr2 < best_dr2){
      best_dr2 = dr2;
      best_pt = part->pt();
    }
  }
  return best_pt;
}

const bool allBut(const std::map<std::string,bool> map, const std::vector<std::string>& nameMatch){

  bool hasFired = false;

  for(auto& n : nameMatch){
    if(map.find(n) != map.end()){
      auto in = map.find(n);
      if(in->second){
	hasFired = true;
      }
    }
  }

  long unsigned int countFires = 0;
  for(auto it = map.begin(); it != map.end(); ++it){
    if(it->second) ++countFires; 
  }

  return (hasFired && nameMatch.size() == countFires) ? true : false;
}

bool matchedToL1Cand(const std::vector<l1t::EGammaRef>& l1Cands, const float scEta, const float scPhi, const double barrel_end_ = 1.4791, const double region_eta_size_ = 0.522, const double region_eta_size_ecap_ = 1, const double region_phi_size_ = 1.044) {
  for (auto const& l1Cand : l1Cands) {
    //ORCA matching method
    double etaBinLow = 0.;
    double etaBinHigh = 0.;
    if (std::abs(scEta) < barrel_end_) {
      etaBinLow = l1Cand->eta() - region_eta_size_ / 2.;
      etaBinHigh = etaBinLow + region_eta_size_;
    } else {
      etaBinLow = l1Cand->eta() - region_eta_size_ecap_ / 2.;
      etaBinHigh = etaBinLow + region_eta_size_ecap_;
    }
 
    float deltaphi = std::abs(scPhi - l1Cand->phi());
    if (deltaphi > TWOPI)
      deltaphi -= TWOPI;
    if (deltaphi > TWOPI / 2.)
      deltaphi = TWOPI - deltaphi;

    if (scEta < etaBinHigh && scEta > etaBinLow && deltaphi < region_phi_size_ / 2.) {
      return true;
    }
  }
  return false;
}

bool matchedToL1CandWithDR(const std::vector<l1t::EGammaRef>& l1Cands, const float scEta, const float scPhi, const float maxDeltaR=0.1) {

  const float maxDR2 = maxDeltaR*maxDeltaR;
  for (auto const& l1Cand : l1Cands) {
    const float DeltaR2 = reco::deltaR2(scEta, scPhi, l1Cand->eta(), l1Cand->phi());
    
    if(DeltaR2 < maxDR2) return true;
  }
  return false;
}

//we need to initalise the menu each run (menu can and will change on run boundaries)
void EfficiencyCalculator::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  hltPSProv_.init(run,setup,hltProcess_,changed);
  //const l1t::L1TGlobalUtil& l1GtUtils = hltPSProv_.l1tGlobalUtil();
  //std::cout <<"l1 menu "<<l1GtUtils.gtTriggerMenuName()<<" version "<<l1GtUtils.gtTriggerMenuVersion()<<" comment "<<std::endl;
  //std::cout <<"hlt name "<<hltPSProv_.hltConfigProvider().tableName()<<std::endl;
}

void EfficiencyCalculator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  edm::Handle<std::vector<trigger::EgammaObject> > ele;
  iEvent.getByToken(eleToken_,ele);

  edm::Handle<edm::TriggerResults > hlt;
  iEvent.getByToken(hltToken_,hlt);

  edm::Handle<trigger::TriggerEvent > hltsevt;
  iEvent.getByToken(hltsevtToken_,hltsevt);

  edm::Handle<std::vector<reco::GenParticle> > gen;
  iEvent.getByToken(genToken_,gen);

  edm::Handle<trigger::TriggerFilterObjectWithRefs > l1seedOutput;
  iEvent.getByToken(L1Token_,l1seedOutput);

  std::vector<l1t::EGammaRef> l1EGs;
  l1seedOutput->getObjects(trigger::TriggerL1EG, l1EGs);


  auto eg_trig_objs_hltEle32WPTightGsfTrackIsoFilter = getListFilterPassedObj("hltEle32WPTightGsfTrackIsoFilter",*hltsevt.product());

  auto genparts = getGenparts(*gen.product());

  //I seem to recall this function being slow so perhaps cache for a given lumi
  //(it only changes on lumi boundaries)
  // int psColumn = hltPSProv_.prescaleSet(iEvent,iSetup);
  // //std::cout <<"PS column "<<psColumn<<std::endl;
  // if(psColumn==0 && iEvent.isRealData()){
  //   std::cout <<"PS column zero detected for data, this is unlikely (almost all triggers are disabled in normal menus here) and its more likely that you've not loaded the correct global tag in "<<std::endl;
  // }

  //note to the reader, what I'm doing is extremely dangerious (a const cast), never do this!
  //however in this narrow case, it fixes a bug in l1t::L1TGlobalUtil (the method should be const)
  //and it is safe for this specific instance
  // l1t::L1TGlobalUtil& l1GtUtils = const_cast<l1t::L1TGlobalUtil&> (hltPSProv_.l1tGlobalUtil());

  // std::cout<<"l1 menu: name decisions prescale "<<std::endl;
  // for(size_t bitNr=0;bitNr<l1GtUtils.decisionsFinal().size();bitNr++){
  // const std::string& bitName = l1GtUtils.decisionsFinal()[bitNr].first; // l1GtUtils.decisionsFinal() is of type std::vector<std::pair<std::string,bool> >

  //   bool passInitial = l1GtUtils.decisionsInitial()[bitNr].second; //before masks and prescales, so if we have a 15 GeV electron passing L1_SingleEG10, it will show up as true but will likely not cause a L1 acccept due to the seeds high prescale
  //   bool passInterm = l1GtUtils.decisionsInterm()[bitNr].second; //after mask (?, unsure what this is)
  //   bool passFinal = l1GtUtils.decisionsFinal()[bitNr].second; //after masks & prescales, true means it gives a L1 accept to the HLT
  //   std::cout << bitName<< " " <<passFinal<<std::endl;
  //   int prescale = l1GtUtils.prescales()[bitNr].second;
  //   std::cout <<"   "<<bitNr<<" "<<bitName<<" "<<passInitial<<" "<<passInterm<<" "<<passFinal<<" "<<prescale<<std::endl;
    
    //Now check if bitName is one of the map keys
    //if(L1seeds.find(bitName) != L1seeds.end() && passFinal) L1seeds[bitName] = true;
    // if( (bitName=="L1_SingleLooseIsoEG26er1p5" || bitName=="L1_SingleLooseIsoEG26er2p5" || bitName=="L1_SingleEG26er2p1" || bitName=="L1_SingleEG26er2p5") && passFinal) passAnyEG26 = true;
    // if(bitName=="L1_SingleIsoEG30er2p5" && passFinal) passSingleIsoEG30er2p5 = true;
    // if( (bitName=="L1_SingleIsoEG28er2p1" || bitName=="L1_SingleIsoEG28er2p5") && passFinal) passSingleIsoEG28er_ = true;
    // if( (bitName=="L1_SingleLooseIsoEG28er1p5" || bitName=="L1_SingleLooseIsoEG28er2p1" || bitName=="L1_SingleLooseIsoEG28er2p5") && passFinal) passSingleLooseIsoEG28er_ = true;
    // if( (bitName=="L1_SingleLooseIsoEG30er1p5" || bitName=="L1_SingleLooseIsoEG30er2p5") && passFinal) passSingleLooseIsoEG30er_ = true;
  //}

  // if(passSingleIsoEG28er_ || passSingleLooseIsoEG28er_) passAnyEG28 = true;
  // if(passSingleIsoEG30er2p5 || passSingleLooseIsoEG30er_) passAnyEG30 = true;
  
  //bool passSingleIsoEG30er2p5 = allBut(L1seeds,SingleIsoEG30er2p5);


  for(auto& eg : genparts){

    if(fabs(eg->eta()) > endcap_end_) continue;

    auto matched_objs_hltEle32WPTightGsfTrackIsoFilter = matchTrigObjs(eg->eta(),eg->phi(),eg_trig_objs_hltEle32WPTightGsfTrackIsoFilter);
    auto nmatch_hltEle32WPTightGsfTrackIsoFilter = matched_objs_hltEle32WPTightGsfTrackIsoFilter.size();

    

    //Fill denominators
      if (fabs(eg->eta()) < 1.44 ) den_ele_pt_EB->Fill(eg->pt());
      if (fabs(eg->eta()) > 1.56 && fabs(eg->eta())<2.4) den_ele_pt_EE->Fill(eg->pt());

    //Fill numerators                                                                                                                                
    if(nmatch_hltEle32WPTightGsfTrackIsoFilter>0){
      //std::cout << "eg phi: " << eg->phi() << std::endl;
      //std::cout << "eg eta: " << eg->eta() << std::endl;
      //std::cout << "eg pt: "  << eg->pt()  << std::endl;
      if (fabs(eg->eta()) < 1.44 ) num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EB->Fill(eg->pt());
      if (fabs(eg->eta()) > 1.56 && fabs(eg->eta())<2.4) num_ele_pt_hltEle32WPTightGsfTrackIsoFilter_EE->Fill(eg->pt());

      bool passL1seeds = matchedToL1CandWithDR(l1EGs,eg->eta(),eg->phi());

      if (fabs(eg->eta()) < 1.44 && passL1seeds) num_ele_pt_passL1seeds_EB->Fill(eg->pt());
      if (fabs(eg->eta()) > 1.56 && fabs(eg->eta())<2.4 && passL1seeds) num_ele_pt_passL1seeds_EE->Fill(eg->pt());

    }
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(EfficiencyCalculator);
