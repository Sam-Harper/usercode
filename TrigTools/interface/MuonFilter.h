#ifndef SHARPER_TRIGTOOLS_MUONFILTER
#define SHARPER_TRIGTOOLS_MUONFILTER


#include "FWCore/Framework/interface/EDFilter.h"
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

class MuonFilter : public edm::EDFilter {
private:
  int nrPass_;

  float ptCut1_,ptCut2_;
  int nrToPassCut1_,nrToPassCut2_;
  edm::InputTag muonTag_;
  bool reverseCuts_;
  
  std::string outputFilename_;
  TTree* tree_;
  TFile* file_;

  struct EvtInfoStruct {
    int runnr,lumiSec,eventNr,bx;
  };
  EvtInfoStruct evtInfo_;
  int nrPassCut1_,nrPassCut2_;
  int nrMu_;
  //std::vector<TLorentzVector> p4Vec_;
  std::vector<float> etaVec_;
  std::vector<float> phiVec_;
  std::vector<float> ptVec_;
  // TClonesArray* p4Array_;
  
private:
  MuonFilter(const MuonFilter& rhs){}
  MuonFilter& operator=(const MuonFilter& rhs){return *this;}

public:
  explicit MuonFilter(const edm::ParameterSet& iPara);
  ~MuonFilter(){delete file_;}//delete p4Array_;}
  
  virtual bool filter(edm::Event& iEvent,const edm::EventSetup& iSetup);
  virtual void endJob();
  

};

#endif
