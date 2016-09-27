#ifndef SHARPER_HEEPANALYZER_HEEPEVENTHELPER
#define SHARPER_HEEPANALYZER_HEEPEVENTHELPER

//class: heep::EventHelper
//Description: a class which constructs heep::Event via the call makeHeepEvent 


#include "FWCore/Utilities/interface/EDGetToken.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEleSelector.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtraFiller.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvtHandles.h"



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
  class Event;

  class EventHelper {
    
     //first necessary labels
    //physics objects
    edm::EDGetTokenT<edm::View<pat::Electron>> eleTag_;
    edm::EDGetTokenT<edm::View<pat::Muon>> muonTag_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetTag_;
    edm::EDGetTokenT<edm::View<pat::Tau>> tauTag_;
    edm::EDGetTokenT<edm::View<pat::MET>> metTag_;
    edm::EDGetTokenT<edm::View<pat::Photon>> phoTag_;
    //non physics objects
    edm::EDGetTokenT<EcalRecHitCollection> ecalRecHitsEBTag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalRecHitsEETag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalReducedRecHitsEBTag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalReducedRecHitsEETag_;
    edm::EDGetTokenT<HBHERecHitCollection> hcalRecHitsTag_;
    edm::EDGetTokenT<edm::View<reco::GsfElectron> > gsfEleTag_;
    edm::EDGetTokenT<edm::View<reco::Photon> > recoPhoTag_;
    edm::EDGetTokenT<reco::PFCandidateCollection> pfCandidateTag_;
    edm::EDGetTokenT<reco::SuperClusterCollection> superClusterEBTag_;
    edm::EDGetTokenT<reco::SuperClusterCollection> superClusterEETag_; 
    edm::EDGetTokenT<reco::PreshowerClusterCollection> preShowerClusterXTag_;
    edm::EDGetTokenT<reco::PreshowerClusterCollection> preShowerClusterYTag_;
    edm::EDGetTokenT<reco::TrackCollection> ctfTrackTag_;
    edm::EDGetTokenT<reco::GenParticleCollection> genParticleTag_;
    edm::EDGetTokenT<trigger::TriggerEvent> trigEventTag_;  
    edm::EDGetTokenT<edm::TriggerResults> trigResultsTag_;
    edm::EDGetTokenT<GenEventInfoProduct> genEventInfoTag_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileUpMCInfoTag_;
    edm::EDGetTokenT<L1GlobalTriggerReadoutRecord> l1RecordTag_;
    edm::EDGetTokenT<l1extra::L1EmParticleCollection> l1EmNonIsoTag_;
    edm::EDGetTokenT<l1extra::L1EmParticleCollection> l1EmIsoTag_;
    edm::EDGetTokenT<reco::VertexCollection> verticesTag_;
    edm::EDGetTokenT<CaloTowerCollection> caloTowersTag_;
    edm::EDGetTokenT<double> eleRhoCorrTag_;
    edm::EDGetTokenT<double> eleRhoCorr2012Tag_;
    edm::EDGetTokenT<bool> ecalLaserFilterTag_;
    edm::EDGetTokenT<edm::ValueMap<double>> pfChargedIsoValEleMapTag_;
    edm::EDGetTokenT<edm::ValueMap<double>> pfPhotonIsoValEleMapTag_; 
    edm::EDGetTokenT<edm::ValueMap<double>> pfNeutralIsoValEleMapTag_;
    edm::EDGetTokenT<edm::ValueMap<std::vector<reco::PFCandidateRef>>> gsfEleToPFCandMapTag_;
    edm::EDGetTokenT<std::vector<reco::PFCluster>> pfClustersECALTag_;
    edm::EDGetTokenT<std::vector<reco::PFCluster>> pfClustersHCALTag_;
    edm::EDGetTokenT<edm::ValueMap<bool>> heepIDVIDTag_;
    edm::EDGetTokenT<reco::RecoEcalCandidateCollection> egHLTCandsTag_;
    edm::EDGetTokenT<reco::RecoEcalCandidateCollection> egHLTCandsUnseededTag_;
    edm::EDGetTokenT<LHEEventProduct> lheEventTag_;
    edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoTag_;
    
    std::shared_ptr<HLTPrescaleProvider> hltPSProvider_;

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

    //right this is a classic 1am hack (okay its only 22:30)
    //because this has both handles and tokens, it doesnt play nice
    //with the current design, poor I know
    mutable heep::GsfEleExtraFiller gsfEleExtraFiller_;

  public:
    template<typename T>
    EventHelper(const edm::ParameterSet& conf,edm::ConsumesCollector && cc,
		T& module){setup(conf,cc,module);}
    EventHelper(){}
    ~EventHelper(){}
    //we own nothing so default copy/assignments are fine
    
    template<typename T>
    void setup(const edm::ParameterSet& conf,edm::ConsumesCollector && cc,
	       T& module){setup(conf,cc,module);}
    template<typename T>
    void setup(const edm::ParameterSet& conf,edm::ConsumesCollector & cc,T& module){
      hltPSProvider_=std::make_shared<HLTPrescaleProvider>(conf,cc,module);
      setup_(conf,cc);
    }
    
    
    //this function just calles setHandles and then fillHEEPElesFromPat but it allows us to make the HEEPEvent in one function
    void makeHeepEvent(const edm::Event& edmEvent,const edm::EventSetup& setup,heep::Event& heepEvent)const;
    void makeHeepEvent(const edm::Run& run,const edm::EventSetup& setup,heep::Event& heepEvent)const;

    //the remaining functions are all called by makeHeepEvent 
    void setHandles(const edm::Event& event,const edm::EventSetup& setup,heep::EvtHandles& handles)const;
    void fillHEEPElesFromPat(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const; 
    void fillHEEPElesFromGsfEles(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const;
    
  private:
    void addHEEPEle_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const;
    //the function to setup the object from the ParameterSet
    void setup_(const edm::ParameterSet& conf,edm::ConsumesCollector && cc){setup_(conf,cc);}
    void setup_(const edm::ParameterSet& conf,edm::ConsumesCollector & cc);
    
    template<typename T>
    void getToken_(edm::EDGetTokenT<T>& token,const edm::InputTag& tag,
		  edm::ConsumesCollector& cc){
      token=cc.consumes<T>(tag);
    }
  };
}


#endif
