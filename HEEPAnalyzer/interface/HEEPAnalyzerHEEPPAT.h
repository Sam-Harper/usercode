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
#include "DataFormats/Math/interface/LorentzVector.h"
#include "TTree.h"
#include <string>

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

  struct DebugTreeStruct{
    float mass;
    int evtRegion;
    int ele1CutCode;
    int ele2CutCode;
    struct EvtInfo{
      int runnr,lumiSec,eventnr;
      static std::string contents(){return "runnr/I:lumiSec:eventnr";}
    };
    EvtInfo evtInfo;
    
    void createBranches(TTree* tree){
      tree->Branch("mass",&mass,"mass/F");
      tree->Branch("evtRegion",&evtRegion,"evtRegion/I");
      tree->Branch("evtInfo",&evtInfo,EvtInfo::contents().c_str());
      tree->Branch("ele1CutCode",&ele1CutCode,"ele1CutCode/I");
      tree->Branch("ele2CutCode",&ele2CutCode,"ele2CutCode/I");
    }
    void setBranchAddresses(TTree* tree){
      tree->SetBranchAddress("mass",&mass);
      tree->SetBranchAddress("evtRegion",&evtRegion);
      tree->SetBranchAddress("evtInfo",&evtInfo);
      tree->SetBranchAddress("ele1CutCode",&ele1CutCode);
      tree->SetBranchAddress("ele2CutCode",&ele2CutCode);
    }
  };

  struct DebugSingleTreeStruct{
    struct P4Struct {
      float nrgy,et,eta,phi;
      static std::string contents(){return "nrgy/F:et:eta:phi";}
      void fill(const math::XYZTLorentzVector& p4){
	nrgy = p4.e();	et = p4.Et();eta = p4.eta();phi = p4.phi();
      }
      void clear(){
	nrgy = -999;et = -999;eta = -999;phi = -999;
      }
    };
    P4Struct p4;
    int region;
    int cutCode;
    struct EvtInfo{
      int runnr,lumiSec,eventnr;
      static std::string contents(){return "runnr/I:lumiSec:eventnr";}
    };
    EvtInfo evtInfo;
    
    void createBranches(TTree* tree){
      tree->Branch("p4",&p4,P4Struct::contents().c_str());
      tree->Branch("region",&region,"region/I");
      tree->Branch("evtInfo",&evtInfo,EvtInfo::contents().c_str());
      tree->Branch("cutCode",&cutCode,"cutCode/I");
    }
    void setBranchAddresses(TTree* tree){
      tree->SetBranchAddress("p4",&p4);
      tree->SetBranchAddress("region",&region);
      tree->SetBranchAddress("evtInfo",&evtInfo);
      tree->SetBranchAddress("cutCode",&cutCode);
    }
  };

  DebugTreeStruct treeData_;	
  DebugSingleTreeStruct singleTreeData_;
  TTree* tree_;
  TTree* singleTree_;


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
