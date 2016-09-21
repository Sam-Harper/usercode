#ifndef SHARPER_SHNTUPLISTER_SHEVENTTREEDATA
#define SHARPER_SHNTUPLISTER_SHEVENTTREEDATA

namespace edm{
  class ParameterSet;
}

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/TrigMenuDataMgr.hh"
#include "TTree.h"

class SHEventTreeData {
public:
  struct BranchData {
    bool addPFCands;
    bool addPFClusters;
    bool addCaloTowers;
    bool addCaloHits;
    bool addIsolTrks;
    bool addPreShowerClusters;
    bool addGenInfo;
    bool addPUInfo;
    bool addTrigSum;
    bool addMet;
    bool addJets;
    bool addMuons;
    bool addSuperClus;
    bool addEles; 
    bool addHLTDebug;    
    bool addMCParts;
    bool filterIsolTrks;
    bool filterEcalHits;
    bool filterHcalHits;
    bool filterCaloTowers;
    BranchData(){}
    BranchData(const edm::ParameterSet& iPara){setup(iPara);}
    void setup(const edm::ParameterSet& iPara);
   
  };

  
private:
  BranchData branches_;

  TTree* tree_;
  SHEvent* & event_;  //we dont own this

  SHPileUpSummary* shPUSum_; //we dont own this
  SHPFCandContainer* shPFCands_;//or this
  SHPFClusterContainer* shPFClusters_;//or this
  SHCaloTowerContainer* shCaloTowers_; //we dont own 
  SHCaloHitContainer* shCaloHits_; //nor do we own this
  TClonesArray* shIsolTrks_;//or this
  TClonesArray* shPreShowerClusters_;//or this
  SHGenInfo* shGenInfo_;//we own this, nah just kidding, we dont
  SHTrigSummary* shTrigSum_; //we do not own this
  
  TrigMenuDataMgr trigMenuMgr_;

public:
  SHEventTreeData(SHEvent* & event);
  void setMemLocs();
  void makeTree(const std::string& name);
  void fill();
  void setup(const edm::ParameterSet& iPara){branches_.setup(iPara);}

};


#endif
