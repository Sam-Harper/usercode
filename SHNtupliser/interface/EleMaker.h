#ifndef SHARPER_SHNTUPLISER_ELEMAKER
#define SHARPER_SHNTUPLISER_ELEMAKER

//a class which reproduces GsfElectron Algo as much as possible
//idea is to be able to make trackless electrons

#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"
#include "SHarper/SHNtupliser/interface/SHEleCMSSWStructs.hh"

#include "TLorentzVector.h"

namespace edm{
  class ParameterSet;
}

class EleMaker {

private:
  double intRadiusTk_;  
  double ptMinTk_;  
  double maxVtxDistTk_; 
  double maxDrbTk_; 
  double intRadiusHcal_;
  double etMinHcal_; 
  double intRadiusEcalBarrel_; 
  double intRadiusEcalEndcaps_;
  double jurassicWidth_; 
  double etMinBarrel_;
  double eMinBarrel_;
  double etMinEndcaps_; 
  double eMinEndcaps_;  
  bool vetoClustered_;  
  bool useNumCrystals_;



public:
  
  void setup(const edm::ParameterSet& iConfig);

  cmssw::IsolationVariables getIsol(const reco::SuperCluster& superClus,const heep::Event& heepEvent,float radius)const;
  cmssw::FiducialFlags getFid(const reco::SuperCluster& superClus,const heep::Event& heepEvent)const; 
  cmssw::ShowerShape getShowerShape(const reco::SuperCluster& superClus,const heep::Event& heepEvent,bool isEB)const;
  TLorentzVector getP4(const reco::SuperCluster& superClus,const heep::Event& heepEvent)const;
};


#endif
