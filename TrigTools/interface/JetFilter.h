#ifndef SHARPER_TRIGTOOLS_JETFILTER
#define SHARPER_TRIGTOOLS_JETFILTER


#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include <vector>
#include <string>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
//#include "TClonesArray.h"

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}

class JetFilter : public edm::one::EDFilter<> {
private:
  int nrPass_;

  float ptCut1_,ptCut2_;
  int nrToPassCut1_,nrToPassCut2_;
  edm::InputTag jetTag_;
  bool reverseCuts_;
  
  std::string outputFilename_;
  TTree* tree_;
  TFile* file_;

  struct EvtInfoStruct {
    int runnr,lumiSec,eventNr,bx;
  };
  EvtInfoStruct evtInfo_;
  int nrPassCut1_,nrPassCut2_;
  int nrJet_;
  //std::vector<TLorentzVector> p4Vec_;
  std::vector<float> etaVec_;
  std::vector<float> phiVec_;
  std::vector<float> ptVec_; 
  std::vector<float> massVec_;
  // TClonesArray* p4Array_;
  
private:
  JetFilter(const JetFilter& rhs){}
  JetFilter& operator=(const JetFilter& rhs){return *this;}

public:
  explicit JetFilter(const edm::ParameterSet& iPara);
  ~JetFilter(){delete file_;}//delete p4Array_;}
  
  virtual bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup);
  virtual void endJob();
  

};

#endif
