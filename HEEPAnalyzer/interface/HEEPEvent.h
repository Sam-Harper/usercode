#ifndef SHARPER_HEEPANALYZER_HEEPEVENT
#define SHARPER_HEEPANALYZER_HEEPEVENT


//struct: HEEP Event
//Description: an event format usefull for the HEEP group
//
//Implimentation: 
//   a class which has all the handles/links to all the common information the heep group needs to analyzer events
//   it is intentionally not stored anywhere and is expected to grow as needed
//   it contains a link the edm::Event to add backwards compatiblity
//   its almost a struct but it trys to hide a little of the complexity so its a class for now
//   header only, no need for .cc file
#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvtHandles.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/Framework/interface/Event.h"

#include <vector>

namespace heep{

  class Event {
  private:
    //our three data members which we do not own if they are pointers
    const edm::Event* edmEvent_;
    heep::EvtHandles handles_;  
    //I cant make a View (comments say its dangerous) so we will have to have a vector instead
    //although a View is a basically a wrapper around a vector
    std::vector<heep::Ele> heepEles_;
     
  public:
    //default copy and assigment operators are fine as we dont own anything
    Event(){}
    ~Event(){}
    
    int runnr()const{return edmEvent_->id().run();}
    int eventnr()const{return edmEvent_->id().event();}

    const edm::View<pat::Muon>& muons()const{return *handles_.muon;}
    const edm::View<pat::Jet>& jets()const{return *handles_.jet;}
    const edm::View<pat::Electron>& patElectrons()const{return *handles_.electron;}
    const edm::View<pat::MET>& mets()const{return *handles_.met;}
    const edm::View<pat::Photon>& photons()const{return *handles_.pho;}
    const edm::View<pat::Tau>& taus()const{return *handles_.tau;}
    //const std::vector<heep::Ele>& electrons()const{return heepEles_;}
    const std::vector<reco::SuperCluster>& superClustersEB()const{return *handles_.superClusEB;}   
    const std::vector<reco::SuperCluster>& superClustersEE()const{return *handles_.superClusEE;}
    const EcalRecHitCollection* ebHitsFull()const{return &(*handles_.ebRecHits);}
    const EcalRecHitCollection* eeHitsFull()const{return &(*handles_.eeRecHits);}
    const HBHERecHitCollection* hbheHits()const{return &(*handles_.hbheRecHits);}
    const std::vector<reco::Track>& ctfTracks()const{return *handles_.ctfTrack;}
    const std::vector<reco::GenParticle>& genParticles()const{return *handles_.genParticle;}

    const edm::Event& event()const{return *edmEvent_;}
    heep::EvtHandles& handles(){return handles_;}
    const heep::EvtHandles& handles()const{return handles_;}
    std::vector<heep::Ele>& heepElectrons(){return heepEles_;}
    const std::vector<heep::Ele>& heepElectrons()const{return heepEles_;}
   
    //our one set method
    void setEvent(const edm::Event& event){edmEvent_ = &event;}
    
  };

}

#endif
