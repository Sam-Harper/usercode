#ifndef SHarper_HEEPAnalyzer_HEEPIdValueMapProducer_h
#define SHarper_HEEPAnalyzer_HEEPIdValueMapProducer_h

// This producer adds makes a value map with the heep selection results
//
//
// Original Author:  S. Harper 
//         Created: Tues Sep 2 2008

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class HEEPIdValueMapProducer : public edm::EDProducer {

private:
  heep::EleSelector cuts_; //allows us to apply the heep selection
  edm::InputTag eleLabel_;
  edm::InputTag eleRhoCorrLabel_;
  bool applyRhoCorrToEleIsol_;
  bool writeIdAsInt_;

public:
  explicit HEEPIdValueMapProducer(const edm::ParameterSet& iPara);
  virtual ~HEEPIdValueMapProducer(){}
  
private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  
};

#endif
