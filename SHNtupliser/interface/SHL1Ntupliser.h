#ifndef SHarper_SHNtupliser_SHL1Ntupliser_h
#define SHarper_SHNtupliser_SHL1Ntupliser_h


#include "SHarper/SHNtupliser/interface/SHNtupliser.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1TCalorimeter/interface/CaloCluster.h"
#include "DataFormats/L1TCalorimeter/interface/CaloTower.h"
#include <string>

class SHL1Event;


class SHL1Ntupliser : public SHNtupliser {

private:
  SHL1Event* shL1Evt_; //base class actually owns the object, this is just the casted version of the pointer
  
  edm::EDGetTokenT<l1t::EGammaBxCollection> l1EgammasToken_;
  edm::EDGetTokenT<l1t::CaloClusterBxCollection> l1CaloClustersToken_;
  edm::EDGetTokenT<l1t::CaloTowerBxCollection> l1CaloTowersToken_;
  


public:
  explicit SHL1Ntupliser(const edm::ParameterSet& iPara);
  virtual ~SHL1Ntupliser();
  
private:
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);

protected:
    //makes this able to store objects inheriting from SHEvent
  virtual void initSHEvent();
  

};

#endif
