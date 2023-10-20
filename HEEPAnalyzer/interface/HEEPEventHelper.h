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

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"

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
    edm::EDGetTokenT<std::vector<pat::PackedCandidate> > packedPFCandTag_;
    edm::EDGetTokenT<std::vector<pat::PackedCandidate> > lostTrackTag_;
    edm::EDGetTokenT<std::vector<pat::PackedCandidate> > lostEleTrackTag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalRecHitsEBTag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalRecHitsEETag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalWeightsRecHitsEBTag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalWeightsRecHitsEETag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalReducedRecHitsEBTag_;
    edm::EDGetTokenT<EcalRecHitCollection> ecalReducedRecHitsEETag_;
    edm::EDGetTokenT<HBHERecHitCollection> hcalRecHitsTag_;
    edm::EDGetTokenT<edm::View<reco::GsfElectron> > gsfEleTag_;
    edm::EDGetTokenT<edm::View<reco::GsfElectron> > oldGsfEleTag_;
    edm::EDGetTokenT<edm::View<reco::Photon> > recoPhoTag_;
    edm::EDGetTokenT<edm::View<reco::Photon> > oldPhoTag_;
    edm::EDGetTokenT<reco::PFCandidateCollection> pfCandidateTag_;
    edm::EDGetTokenT<reco::SuperClusterCollection> superClusterEBTag_;
    edm::EDGetTokenT<reco::SuperClusterCollection> superClusterEETag_; 
    edm::EDGetTokenT<reco::PreshowerClusterCollection> preShowerClusterXTag_;
    edm::EDGetTokenT<reco::PreshowerClusterCollection> preShowerClusterYTag_;
    edm::EDGetTokenT<reco::TrackCollection> ctfTrackTag_;
    edm::EDGetTokenT<reco::GenParticleCollection> genParticleTag_;
    edm::EDGetTokenT<std::vector<pat::PackedGenParticle> > packedGenParticleTag_;
    edm::EDGetTokenT<trigger::TriggerEvent> trigEventTag_;  
    edm::EDGetTokenT<std::vector<pat::TriggerObjectStandAlone> > patTrigObjsTag_;
    edm::EDGetTokenT<edm::TriggerResults> trigResultsTag_;
    edm::EDGetTokenT<GenEventInfoProduct> genEventInfoTag_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileUpMCInfoTag_;
    edm::EDGetTokenT<L1GlobalTriggerReadoutRecord> l1RecordTag_;
    edm::EDGetTokenT<reco::VertexCollection> verticesTag_;
    edm::EDGetTokenT<CaloTowerCollection> caloTowersTag_;
    edm::EDGetTokenT<double> eleRhoCorrTag_;
    edm::EDGetTokenT<double> eleRhoCorr2012Tag_;
    edm::EDGetTokenT<bool> ecalLaserFilterTag_;
    edm::EDGetTokenT<edm::ValueMap<double>> pfChargedIsoValEleMapTag_;
    edm::EDGetTokenT<edm::ValueMap<double>> pfPhotonIsoValEleMapTag_; 
    edm::EDGetTokenT<edm::ValueMap<double>> pfNeutralIsoValEleMapTag_;
    edm::EDGetTokenT<edm::ValueMap<float>> eleIsolPtTrksValueMapTag_;
    edm::EDGetTokenT<edm::ValueMap<std::vector<reco::PFCandidateRef>>> gsfEleToPFCandMapTag_;
    edm::EDGetTokenT<std::vector<reco::PFCluster>> pfClustersECALTag_;
    edm::EDGetTokenT<std::vector<reco::PFCluster>> pfClustersHCALTag_;
    edm::EDGetTokenT<edm::ValueMap<bool>> heepIDVIDTag_;
    edm::EDGetTokenT<edm::ValueMap<unsigned int>> heepIDVIDBitsTag_;
    std::vector<edm::EDGetTokenT<edm::ValueMap<unsigned int>>> vidBitsTags_;
    edm::EDGetTokenT<reco::RecoEcalCandidateCollection> egHLTCandsTag_;
    edm::EDGetTokenT<reco::RecoEcalCandidateCollection> egHLTCandsUnseededTag_;
    edm::EDGetTokenT<LHEEventProduct> lheEventTag_;
    edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoTag_;
    edm::EDGetTokenT<bool> gsFixDupECALClustersTag_;
    edm::EDGetTokenT<DetIdCollection> gsFixHitsNotReplacedTag_;
    edm::EDGetTokenT<EcalRecHitCollection> gsFixOrgReducedEGEBHitsTag_;
    edm::EDGetTokenT<edm::View<pat::MET> > gsFixMETOrgTag_;
    edm::EDGetTokenT<edm::View<pat::MET> > gsFixMETEGCleanTag_;
    std::vector<edm::EDGetTokenT<edm::View<pat::Jet> > > extraJetTags_;
    std::vector<edm::EDGetTokenT<edm::View<reco::MET> > > extraMETTags_;

    edm::ESGetToken<CaloGeometry,CaloGeometryRecord> caloGeom_;
    edm::ESGetToken<CaloTopology,CaloTopologyRecord> caloTopology_;
    edm::ESGetToken<MagneticField,IdealMagneticFieldRecord> bField_;
    edm::ESGetToken<EcalADCToGeVConstant,EcalADCToGeVConstantRcd> ecalADCToGeV_;
    edm::ESGetToken<EcalLaserDbService,EcalLaserDbRecord> ecalLaser_;
    edm::ESGetToken<EcalIntercalibConstants,EcalIntercalibConstantsRcd> ecalInterCalib_;
    
    
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
    template<typename T>
    void getToken_(std::vector<edm::EDGetTokenT<T> >& tokens,const std::vector<edm::InputTag>& tags,
		  edm::ConsumesCollector& cc){
      tokens.clear();
      for(auto& tag : tags){
	tokens.push_back(cc.consumes<T>(tag));
      }
    }
   
    
    template<typename T>
    static void setHandles(const edm::Event& event,const std::vector<edm::EDGetTokenT<T> >& tokens,
			   std::vector<edm::Handle<T> >& handles){
      if(handles.size()!=tokens.size()) handles.resize(tokens.size());
      for(size_t collNr=0;collNr<tokens.size();collNr++){
	event.getByToken(tokens[collNr],handles[collNr]);
      }
    }

  };
}


#endif
