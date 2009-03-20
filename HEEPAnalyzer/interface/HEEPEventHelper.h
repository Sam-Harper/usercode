#ifndef SHARPER_HEEPANALYZER_HEEPEVENTHELPER
#define SHARPER_HEEPANALYZER_HEEPEVENTHELPER

//class: heep::EventHelper
//Description: a class which constructs heep::Event via the call makeHeepEvent 

#include "FWCore/ParameterSet/interface/InputTag.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"

#include <vector>

namespace reco{
  class BasicCluster;
}

namespace pat{
  class Electron;
}

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}

namespace heep {
  class EvtHandles;
  class Event;

  class EventHelper {
    
     //first necessary labels
    //physics objects
    edm::InputTag eleLabel_;
    edm::InputTag muoLabel_;
    edm::InputTag jetLabel_;
    edm::InputTag tauLabel_;
    edm::InputTag metLabel_;
    edm::InputTag phoLabel_;
    //non physics objects
    edm::InputTag ecalRecHitsEBTag_;
    edm::InputTag ecalRecHitsEETag_;
    edm::InputTag ecalReducedRecHitsEBTag_;
    edm::InputTag ecalReducedRecHitsEETag_;
    edm::InputTag hcalRecHitsTag_;
    edm::InputTag superClusterEBTag_;
    edm::InputTag superClusterEETag_; 
    edm::InputTag ctfTrackTag_;
    edm::InputTag genParticleTag_;
    edm::InputTag trigEventTag_;  
    edm::InputTag genEventPtHatTag_;
    //trigger matching parameters
    std::string hltProcName_;
    double maxDRTrigMatch_;
    double maxPtRelDiffTrigMatch_;
    std::vector<std::string> hltFiltersToCheck_;
    //the selection object we need
    heep::EleSelector cuts_;
    
  public:
    EventHelper(){}
    ~EventHelper(){}
    //we own nothing so default copy/assignments are fine

    //the function to setup the object from the ParameterSet
    void setup(const edm::ParameterSet& conf);

    //this function just calles setHandles and then fillHEEPElesFromPat but it allows us to make the HEEPEvent in one function
    void makeHeepEvent(const edm::Event& edmEvent,const edm::EventSetup& setup,heep::Event& heepEvent)const;
    

    //the remaining functions are all called by makeHeepEvent 
    void setHandles(const edm::Event& event,const edm::EventSetup& setup,heep::EvtHandles& handles)const;
    void fillHEEPElesFromPat(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const; 
    void addHEEPEle(const pat::Electron& patEle,const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const;
    void fillIsolData(const pat::Electron &patEle,heep::Ele::IsolData& isolData)const;
    void fillClusShapeData(const pat::Electron &patEle,heep::Ele::ClusShapeData& isolData)const;
    void fillClusShapeData(const reco::BasicCluster& seedClus,const heep::EvtHandles& handles,heep::Ele::ClusShapeData& clusShapeData)const; //legacy function from before the time pat had cluster shape variables

  };
}


#endif
