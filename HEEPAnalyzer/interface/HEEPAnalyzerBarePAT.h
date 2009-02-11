#ifndef SHarper_HEEPAnalyzer_HEEPAnalyzerBarePat_h
#define SHarper_HEEPAnalyzer_HEEPAnalyzerBarePat_h



// Description: A simple example HEEPAnalyzer which makes a mass spectrum of selected electron pairs

// Implementation: 
//    This does it using just bare PAT with only heep::EleSelector which does the heep selection
//
 
//
// Original Author:  S. Harper
//         Created: Tues Sep 2 2008





#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"


class TH1;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}


class HEEPAnalyzerBarePAT : public edm::EDAnalyzer {

private:
  heep::EleSelector cuts_; //allows us to apply the heep selection
  edm::InputTag eleLabel_;

  //the next three variables are simply for the example analysis
  int nrPass_;
  int nrFail_;
  TH1* massHist_; //we do not own hist

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  HEEPAnalyzerBarePAT(const HEEPAnalyzerBarePAT& rhs){}
  HEEPAnalyzerBarePAT& operator=(const HEEPAnalyzerBarePAT& rhs){return *this;}

public:
  explicit HEEPAnalyzerBarePAT(const edm::ParameterSet& iPara);
  virtual ~HEEPAnalyzerBarePAT(){}
  
private:
  virtual void beginJob(const edm::EventSetup& iSetup) ;
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob() ;
  
};

#endif
