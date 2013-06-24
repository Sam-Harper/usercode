#ifndef SHL1EVENT
#define SHL1EVENT

//event class which stores a few extra L1 specific things
#include "SHarper/SHNtupliser/interface/SHEvent.hh"

#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"
#include "SHarper/SHNtupliser/interface/SHL1CaloTowerContainer.hh"

class SHL1Event : public SHEvent {
private:
  
  TClonesArray l1ClusArray_;
  SHL1CaloTowerContainer l1CaloTowers_;
  
  float l1RhoMedian_;
  float l1RhoRTMean_;
  float l1RhoTLTMean_;

public:
  SHL1Event();
  virtual ~SHL1Event();
 
  const SHL1Cluster* getL1Clus(int clusNr)const;
 
  int nrL1Clus()const{return l1ClusArray_.GetLast()+1;}

  void addL1CaloTowers(const SHL1Event& rhs){l1CaloTowers_ = rhs.l1CaloTowers_;}
  void addL1CaloTowers(const SHL1CaloTowerContainer& towers){l1CaloTowers_ = towers;}
  void addL1Clus(const SHL1Cluster& clus);
  
  const SHL1CaloTowerContainer& getL1CaloTowers()const{return l1CaloTowers_;}
  SHL1CaloTowerContainer& getL1CaloTowers(){return l1CaloTowers_;}
  
  void copyEventPara(const SHL1Event& rhs);
  void copyEventPara(const SHEvent& rhs);
  void clear();
  void flushTempData()const;

  ClassDef(SHL1Event,1)
};

#endif
