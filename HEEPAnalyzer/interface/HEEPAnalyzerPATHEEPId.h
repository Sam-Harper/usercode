#ifndef SHarper_HEEPAnalyzer_HEEPAnalyzerBarePat_h
#define SHarper_HEEPAnalyzer_HEEPAnalyzerBarePat_h



// Description: A simple example HEEPAnalyzer which makes a mass spectrum of selected electron pairs

// Implementation: 
//    This does it using just bare PAT with only heep::EleSelector which does the heep selection
//
 
//
// Original Author:  S. Harper
//         Created: Tues Sep 2 2008





#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"


class TH1;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}


class HEEPAnalyzerPATHEEPId : public edm::one::EDAnalyzer<> {

private:
 
  edm::InputTag eleLabel_;

  //the next three variables are simply for the example analysis
  int nrPass_;
  int nrFail_;
  TH1* massHist_; //we do not own hist

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  HEEPAnalyzerPATHEEPId(const HEEPAnalyzerPATHEEPId& rhs){}
  HEEPAnalyzerPATHEEPId& operator=(const HEEPAnalyzerPATHEEPId& rhs){return *this;}

public:
  explicit HEEPAnalyzerPATHEEPId(const edm::ParameterSet& iPara);
  virtual ~HEEPAnalyzerPATHEEPId(){}
  
private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob() ;
  
};

#endif
