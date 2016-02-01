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
#include "SHarper/HEEPAnalyzer/interface/HEEPTrigCodes.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include <vector>

namespace heep{

  class Event {
  private:
    //our five data members which we do not own if they are pointers
    const edm::Event* edmEvent_;
    const edm::EventSetup* edmEventSetup_;
    heep::EvtHandles handles_;  
    //in 76X, hltConfig needs to be initialised with the modulename + consumes collectors
    //at construction time
    //I've decided to delgate this to the heep event helper class, hence why hltConfig is 
    //now a (smart) pointer
    //because HLTPrecaleProvider cant be copied and I didnt want to destroy the heep::EventHelpers copy, I've made it shared with the heep::EventHelper, I dont like this design and may
    //change it in the future
    std::shared_ptr<HLTPrescaleProvider> hltPSProvider_;
    //I cant make a View (comments say its dangerous) so we will have to have a vector instead
    //although a View is a basically a wrapper around a vector
    std::vector<heep::Ele> heepEles_;
    heep::TrigCodes::TrigBitSet trigBits_; //the trigger bit corresponding to the triggers we are interested in
  public:
    //default copy and assigment operators are fine as we dont own anything
    Event(){}
    ~Event(){}
    
    int runnr()const{return edmEvent_->id().run();}
    int eventnr()const{return edmEvent_->id().event();} 
    int bx()const{return edmEvent_->bunchCrossing();}
    int lumiSec()const{return edmEvent_->luminosityBlock();}
    unsigned long long time()const{return edmEvent_->time().value();}
    int orbitNumber()const{return edmEvent_->orbitNumber();}

    const heep::TrigCodes::TrigBitSet& trigBits()const{return trigBits_;}
    bool passAllTrigs(heep::TrigCodes::TrigBitSet& bitsToCheck){return (bitsToCheck&trigBits_)==bitsToCheck;}
    bool passAnyTrig(heep::TrigCodes::TrigBitSet& bitsToCheck){return (bitsToCheck&trigBits_)!=0x0;}
   
    const edm::View<pat::Muon>& muons()const{return *handles_.muon;}
    const edm::View<pat::Jet>& jets()const{return *handles_.jet;}
    const edm::View<pat::Electron>& patEles()const{return *handles_.electron;} 
    const edm::View<pat::MET>& mets()const{return *handles_.met;}
    const edm::View<pat::Photon>& photons()const{return *handles_.pho;}
    const edm::View<pat::Tau>& taus()const{return *handles_.tau;}
    const std::vector<reco::SuperCluster>& superClustersEB()const{return *handles_.superClusEB;}   
    const std::vector<reco::SuperCluster>& superClustersEE()const{return *handles_.superClusEE;}
    const std::vector<reco::GsfElectron>& gsfEles()const{return *handles_.gsfEle;}
    const std::vector<reco::Photon>& recoPhos()const{return *handles_.recoPho;}
    const std::vector<reco::PFCandidate>& pfCands()const{return *handles_.pfCandidate;}
    const EcalRecHitCollection* ebHitsFull()const{return &(*handles_.ebRecHits);}
    const EcalRecHitCollection* eeHitsFull()const{return &(*handles_.eeRecHits);}
    const HBHERecHitCollection* hbheHits()const{return &(*handles_.hbheRecHits);}
    const std::vector<reco::Track>& ctfTracks()const{return *handles_.ctfTrack;}
    const std::vector<reco::GenParticle>& genParticles()const{return *handles_.genParticle;}
    bool hasGenParticles()const{return handles_.genParticle.isValid();}

    const edm::Event& event()const{return *edmEvent_;}
    const edm::EventSetup& eventSetup()const{return *edmEventSetup_;}
    int hltPreScale(const std::string& hltPath)const{return hltPSProvider_ && hltPSProvider_->hltConfigProvider().inited() ? hltPSProvider_->prescaleValue(event(),eventSetup(),hltPath) : -1;}
    int preScaleColumn()const{return hltPSProvider_ && hltPSProvider_->hltConfigProvider().inited() ? hltPSProvider_->prescaleSet(event(),eventSetup()) : -1;}
    double eleRhoCorr()const{return *handles_.eleRhoCorr;}
    double eleRhoCorr2012()const{return *handles_.eleRhoCorr2012;}
    double eleRhoCorr2011()const{return *handles_.eleRhoCorr;}
    bool passEcalLaserFilter(bool passIfNotFound=true)const{return handles_.ecalLaserFilter.isValid() ? *handles_.ecalLaserFilter : passIfNotFound;}
    heep::EvtHandles& handles(){return handles_;}
    const heep::EvtHandles& handles()const{return handles_;}
    std::vector<heep::Ele>& heepEles(){return heepEles_;}
    const std::vector<heep::Ele>& heepEles()const{return heepEles_;}
    const trigger::TriggerEvent & triggerEvent()const{return *handles_.trigEvent;}
    double genEventPtHat()const{return handles_.genEventInfo.isValid() ? handles_.genEventInfo->qScale(): -1.;}
    const std::vector<bool> l1Decision()const{return handles_.l1Record->decisionWord();} //as decisionWord returns by value so we do to
    const std::vector<l1extra::L1EmParticle>& l1EmNonIso()const{return *handles_.l1EmNonIso;}
    const std::vector<l1extra::L1EmParticle>& l1EmIso()const{return *handles_.l1EmIso;}
    const std::vector<reco::PFCluster>& pfClustersECAL()const{return *handles_.pfClustersECAL;}
    const std::vector<reco::PFCluster>& pfClustersHCAL()const{return *handles_.pfClustersHCAL;}
    
    //really for heep::EventHelper to make the HLTPSProvider object
    bool validHLTPSProvider()const{return hltPSProvider_!=nullptr;}
    HLTPrescaleProvider* hltPSProv()const{return hltPSProvider_.get();}

     //our four set methods   
    void setEvent(const edm::Event& event){edmEvent_ = &event;}
    void setEventSetup(const edm::EventSetup& setup){edmEventSetup_=&setup;}
    void setTrigBits(heep::TrigCodes::TrigBitSet& bits){trigBits_=bits;}
    void setHLTPSProvider(const std::shared_ptr<HLTPrescaleProvider>& psProv){hltPSProvider_=psProv;}

    
    bool initHLTConfig(const edm::Run& run,const edm::EventSetup& setup,const std::string& hltProcess){
      bool changed=false;
      if(hltPSProvider_) hltPSProvider_->init(run,setup,hltProcess,changed);
      return changed;
    }
    
  };

}

#endif
