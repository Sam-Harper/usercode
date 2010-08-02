#ifndef SHarper_HEEPAnalyzer_HEEPAnalyzerHEEPPat_h
#define SHarper_HEEPAnalyzer_HEEPAnalyzerHEEPPat_h



// Description: A simple example HEEPAnalyzer which makes a mass spectrum of selected electron pairs

// Implementation: 
//    This does it using just bare PAT with only heep::EleSelector which does the heep selection
//
 
//
// Original Author:  S. Harper
//         Created: Tues Sep 2 2008





#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"


class TH1;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

//this analyser shows you how to use pat electrons that have been remade by HEEPAttStatusToPAT to have 
//the heep ID in and the energy of the electron used for the p4 to be reset to the ecal energy

class HEEPAnalyzerHEEPPAT : public edm::EDAnalyzer {

private:

  edm::InputTag eleLabel_;

  //the next three variables are simply for the example analysis
  int nrPass_;
  int nrFail_;
  TH1* massHist_; //we do not own hist

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  HEEPAnalyzerHEEPPAT(const HEEPAnalyzerHEEPPAT& rhs){}
  HEEPAnalyzerHEEPPAT& operator=(const HEEPAnalyzerHEEPPAT& rhs){return *this;}

public:
  explicit HEEPAnalyzerHEEPPAT(const edm::ParameterSet& iPara);
  virtual ~HEEPAnalyzerHEEPPAT(){}
  
private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob() ;
  
};

#endif
