#ifndef SHarper_HEEPAnalyzer_HEEPAttStatusToPAT_h
#define SHarper_HEEPAnalyzer_HEEPAttStatusToPAT_h

// This producer adds the HEEPSelector status word as userdate to the PAT electron collection
//
//
// Original Author:  S. Harper / M.Mozer
//         Created: Tues Sep 2 2008

#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class HEEPAttStatusToPAT : public edm::EDProducer {

private:
  heep::EleSelector cuts_; //allows us to apply the heep selection
  edm::InputTag eleLabel_;

public:
  explicit HEEPAttStatusToPAT(const edm::ParameterSet& iPara);
  virtual ~HEEPAttStatusToPAT(){}
  
private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  
};

#endif
