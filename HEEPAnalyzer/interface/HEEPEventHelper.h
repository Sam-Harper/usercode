#ifndef SHARPER_HEEPANALYZER_HEEPEVENTHELPER
#define SHARPER_HEEPANALYZER_HEEPEVENTHELPER

//class: heep::EventHelper
//Description: a class which constructs heep::Event via the call makeHeepEvent 

#include "FWCore/Utilities/interface/InputTag.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEffectiveAreas.h"

#include <vector>

namespace reco{
  class CaloCluster;
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
    edm::InputTag gsfEleTag_;
    edm::InputTag recoPhoTag_;
    edm::InputTag superClusterEBTag_;
    edm::InputTag superClusterEETag_; 
    edm::InputTag ctfTrackTag_;
    edm::InputTag genParticleTag_;
    edm::InputTag trigEventTag_;  
    edm::InputTag trigResultsTag_;
    edm::InputTag genEventInfoTag_;
    edm::InputTag pileUpMCInfoTag_;
    edm::InputTag l1RecordTag_;
    edm::InputTag l1EmNonIsoTag_;
    edm::InputTag l1EmIsoTag_;
    edm::InputTag verticesTag_;
    edm::InputTag caloTowersTag_;
    edm::InputTag eleRhoCorrTag_;
    edm::InputTag pfChargedIsoValEleMapTag_;
    edm::InputTag pfPhotonIsoValEleMapTag_; 
    edm::InputTag pfNeutralIsoValEleMapTag_;

    //trigger matching parameters
    std::string hltProcName_;
    double maxDRTrigMatch_;
    double maxPtRelDiffTrigMatch_;
    std::vector<std::string> hltFiltersToCheck_;
    std::vector<std::pair<std::string,int> > hltFiltersToCheckWithNrCands_;
    //the selection object we need
    heep::EleSelector cuts_;
    
    //if true we only promote ecal driven electrons to heep electron status
    bool onlyAddEcalDriven_;
    int heepEleSource_; // 0=fill from GsfElectrons, 1 fill from pat::Electrons

    //isolation correction parameters
    bool applyRhoCorrToEleIsol_;
    heep::EffectiveAreas eleIsolEffectiveAreas_;

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
    void fillHEEPElesFromGsfEles(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const;
    
  private:
    void addHEEPEle_(const reco::GsfElectron& gsfEle,const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const;
   
  };
}


#endif
