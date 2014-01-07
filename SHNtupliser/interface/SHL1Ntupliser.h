#ifndef SHarper_SHNtupliser_SHL1Ntupliser_h
#define SHarper_SHNtupliser_SHL1Ntupliser_h


#include "SHarper/SHNtupliser/interface/SHNtupliser.h"
#include <string>

class SHL1Event;


class SHL1Ntupliser : public SHNtupliser {

private:
  SHL1Event* shL1Evt_; //base class actually owns the object, this is just the casted version of the pointer
  
  edm::InputTag l1CaloClustersTag_;
  edm::InputTag l1CaloTowersTag_;

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
