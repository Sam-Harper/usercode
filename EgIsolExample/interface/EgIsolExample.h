#ifndef SHarper_EgIsolExample_EgIsolExample_h
#define SHarper_EgIsolExample_EgIsolExample_h

// Description: A simple example of how to access electron isolations in 21X
//              It creates and fills a histogram (seperated into barrel/endcap) 
//              for each of the 5 isolation for all electrons in the event

//
// Original Author:  Sam Harper (RAL)
//         Created: Fri Nov 28 2008


#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/ValueMap.h"

class TH1;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}


class EgIsolExample : public edm::EDAnalyzer {

private:
  //the input tag to pick up the electrons and value maps containing the isolations
  edm::InputTag electronTag_;
  edm::InputTag ecalIsolTag_;
  edm::InputTag hcalIsolTag_;
  edm::InputTag trackIsolTag_;
  edm::InputTag hcalDepth1IsolTag_;
  edm::InputTag hcalDepth2IsolTag_;

  //here are some example histograms which I'm going to fill with all the appropriate isolations
  //we do not own these histograms, this TFileService will
  TH1* ecalIsolEBHist_;
  TH1* hcalIsolEBHist_;
  TH1* trackIsolEBHist_;
  TH1* hcalDepth1IsolEBHist_;
  TH1* hcalDepth2IsolEBHist_;
  TH1* ecalIsolEEHist_;
  TH1* hcalIsolEEHist_;
  TH1* trackIsolEEHist_;
  TH1* hcalDepth1IsolEEHist_;
  TH1* hcalDepth2IsolEEHist_;

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  EgIsolExample(const EgIsolExample& rhs){}
  EgIsolExample& operator=(const EgIsolExample& rhs){return *this;}

public:
  explicit EgIsolExample(const edm::ParameterSet& iPara);
  virtual ~EgIsolExample(){}
  
  virtual void beginJob(const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob();
  
  const edm::ValueMap<double>& getValueMap(const edm::Event& iEvent,edm::InputTag& inputTag);

};

#endif
