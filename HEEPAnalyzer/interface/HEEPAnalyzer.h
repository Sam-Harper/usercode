#ifndef SHarper_HEEPAnalyzer_HEEPAnalyzer_h
#define SHarper_HEEPAnalyzer_HEEPAnalyzer_h



// Description: A simple example HEEPAnalyzer which makes a mass spectrum of selected electron pairs

// Implementation: 
//    The magic all happens in heep::EventHelper which makes a heep::Event. This contains all the links to 
//    things like superclusters and pat::Electrons as well as the heep::Ele wrappers. 
//
 
//
// Original Author:  S. Harper
//         Created: Tues Sep 2 2008





#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEventHelper.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

class TH1;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}


class HEEPAnalyzer : public edm::one::EDAnalyzer<> {

private:
  heep::EventHelper evtHelper_; //this is our magic class where all the nastyness is contained
  heep::Event heepEvt_;

  //the next three variables are simply for the example analysis
  int nrPass_;
  int nrFail_;
  TH1* massHist_; //we do not own hist

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  HEEPAnalyzer(const HEEPAnalyzer& rhs){}
  HEEPAnalyzer& operator=(const HEEPAnalyzer& rhs){return *this;}

public:
  explicit HEEPAnalyzer(const edm::ParameterSet& iPara);
  virtual ~HEEPAnalyzer(){}
  
private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob() ;
  
};

#endif
