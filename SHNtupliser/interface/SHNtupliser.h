#ifndef SHarper_SHNtupliser_SHNtupliser_h
#define SHarper_SHNtupliser_SHNtupliser_h



// Description: Converts a edm::Event to a heep::Event which is then converted to a SHEvent and written to a TTree

// Implementation:
//   heep::Event in HEEPAnalyzer does all the heavy lifting, all the construction of quantities of the SHEvent is done there
//   all the functions in this package do is translate this information to the SH objects
//   it is a design decision that *no* calculation of variables is done here, everything accessible in a SHEvent must be accessable
//   in a heep::Event, if we need something new, heep::Event is extended
//   As a bonus, the analyzer will optionally write out the Ecal and Hcal geometries to the file. In the future, other information
//   may also be written out
//   The one exception to information not being in heep::Event is information internal to my ntuples (such as datasetCode,eventWeight, isMC) 
 
//
// Original Author:  S. Harper
//         Created: Tues Sep 16 2008



#include "SHarper/SHNtupliser/interface/SHEventHelper.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEventHelper.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

#include "SHarper/SHNtupliser/interface/SHPileUpSummary.hh"

#include <string>

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

class SHEvent;
class SHCaloTowerContainer;
class SHCaloHitContainer;
class SHPFCandContainer;
class SHGenInfo;
class TTree;
class TFile;
class SHPFClusterContainer;

class TrigDebugObjHelper;
class SHTrigObjContainer;

class SHNtupliser : public edm::EDAnalyzer {

private:
  heep::EventHelper evtHelper_; //this is our magic class where all the nastyness is contained
  heep::Event heepEvt_;

  SHEventHelper shEvtHelper_;
protected:
  SHEvent* shEvt_; //pointer because thats what root likes, we own it, its protected as derived classes need to set it (and they have a pointer the derived SHEvent so theres no point protecting access, they have it...)
private:
  SHCaloTowerContainer* shCaloTowers_; //pointer to caloTowers of shEvt (blame root)
  SHCaloHitContainer* shCaloHits_;
  TClonesArray* shIsolTrks_;
  TClonesArray* shPreShowerClusters_;
  SHGenInfo* shGenInfo_; 

  TTree* evtTree_; //the outFile owns it
  TFile* outFile_; //we own it

  std::string outputFilename_;
  int nrTot_;
  int nrPass_;

  bool initGeom_;

  bool outputGeom_; //write out geom to file
  float minSCEtToPass_;
  int minNrSCToPass_;
  float minJetEtToPass_;
  int minNrJetToPass_;
  
  bool addCaloTowers_;
  bool addCaloHits_;
  bool addIsolTrks_;
  bool addPFCands_;
  bool addPreShowerClusters_;
  bool addPFClusters_;
  bool addGenInfo_;
  
  bool useHLTDebug_;
  bool compTwoMenus_;
  std::string hltTag_;
  std::string secondHLTTag_;
  TrigDebugObjHelper* trigDebugHelper_;
  SHTrigObjContainer* shTrigObjs_;
  SHTrigObjContainer* shTrigObjs2ndTrig_;
  SHEvent* shEvt2ndTrig_;

  SHPileUpSummary* puSummary_;
  bool writePUInfo_;
  
  bool writePDFInfo_;
  std::vector<double> pdfWeightsVec_;
  
  SHPFCandContainer* shPFCands_; 
  SHPFClusterContainer* shPFClusters_;
  // float oldSigmaIEtaIEta_,newSigmaIEtaIEta_,affectedByCaloNavBug_,scNrgy_,scEta_,scPhi_,scEt_;
  //TTree* scTree_;
  //disabling copy and assignment 
  //I cant think of a reason why I would want to copy this class and its complicated to do right due to TTree
private:
  SHNtupliser(const SHNtupliser& rhs){}
  SHNtupliser& operator=(const SHNtupliser& rhs){return *this;}

public:
  explicit SHNtupliser(const edm::ParameterSet& iPara);
  virtual ~SHNtupliser();
  
private:
  virtual void beginJob() ;
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endRun(edm::Run const& iRun, edm::EventSetup const&);
  virtual void endJob() ;

protected:
  bool fillSHEvent(const edm::Event& iEvent,const edm::EventSetup& iSetup);
  void fillTree();
  
  //makes this able to store objects inheriting from SHEvent
  virtual void initSHEvent();
  
  

};

#endif
