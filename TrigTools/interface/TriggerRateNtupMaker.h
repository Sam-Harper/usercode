#ifndef SHarper_TrigTools_TriggerRateNtupMaker_h
#define SHarper_TrigTools_TriggerRateNtupMaker_h

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "TLorentzVector.h"

#include <string>
#include <vector>

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}
namespace trigger{
  class TriggerEvent;
}

class TTree;
class TFile;



class TriggerRateNtupMaker : public edm::EDAnalyzer {
  

private:
  TTree* trigTree_; //the outFile owns it
  TFile* outputFile_; //we own it

  //input parameters
  std::string outputFilename_;
  edm::InputTag trigEventTag_;

  std::vector<std::pair<trigtools::PathData,std::string> > paths_;
  int nrVerts_;
  float zVtx_;
  float pfMET_;
  trigtools::EvtInfoStruct evtInfo_; 

  

  //disabling copy and assignment 
  //I cant think of a reason why I would want to copy this class and its complicated to do right due to the TTree
private:
  TriggerRateNtupMaker(const TriggerRateNtupMaker& rhs){}
  TriggerRateNtupMaker& operator=(const TriggerRateNtupMaker& rhs){return *this;}

public:
  explicit TriggerRateNtupMaker(const edm::ParameterSet& iPara);
  virtual ~TriggerRateNtupMaker();
  
private:
  virtual void beginJob() ;
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup){}
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob() ;
  
 

};

#endif
