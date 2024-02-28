#ifndef SHARPER_SHNTUPLISER_SHEVENTHELPER
#define SHARPER_SHNTUPLISER_SHEVENTHELPER

//class: SHEventHelper
//Description: a class which converts a heep::Event to a SHEvent
//
//Implimentation: 
// 
//Author: Sam Harper

//note: as of 16/09/08, we dont have full functionality, some functions are empty

#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"
#include "SHarper/SHNtupliser/interface/SHEventTreeData.h"
#include "SHarper/SHNtupliser/interface/SHTrigSumMaker.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
//#include "RecoEgamma/EgammaTools/interface/GainSwitchTools.h"
#include "FWCore/Framework/interface/GetterOfProducts.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ProcessMatch.h"

#include <vector>
#include <memory>
namespace heep{
  class Event;
}
namespace edm{

  class Run;
  class EventSetup;
  class ConsumesCollector;
}

namespace reco{
  class TrackBase;
  class EcalRecHit;
}
namespace pat{
  class PackedCandidate;
}
class EcalRecHit;
class CaloTopology;
class SHEvent;

class SHEventHelper {

private:
  int datasetCode_; //an internal code to my ntuples which only means something to me
  float eventWeight_; //the weight of the event, again only really means something to me
  bool isMC_;// if we are running on mc or not
  
  float minEtToPromoteSC_;
  float minEtToSaveEle_;
  bool fillFromGsfEle_;
  bool applyMuonId_;

  SHEventTreeData::BranchData branches_;
  edm::GetterOfProducts<reco::RecoEcalCandidateIsolationMap> getterOfProducts_;
  mutable SHTrigSumMaker trigSumMaker_;

  //storage for SHCaloHits so we dont have to keep reallocating the memory 
  mutable std::vector<SHCaloHit> ecalHitVec_;
  mutable std::vector<SHCaloHit> hcalHitVec_;
  
  //magic numbers, number of hcal cells (need to fix)
  static constexpr int kNrHcalBarrelCrys_=1296*2;
  static constexpr int kNrHcalEndcapCrys_=3384*2;

  static constexpr float kMaxDRTrks_=0.4;
  static constexpr float kMaxDRPFCands_=0.5;
  static constexpr float kMaxDRPFClusts_=0.5;
  static constexpr float kMaxDREcalHits_=0.5;
  static constexpr float kMaxDRHcalHits_=0.5;
  static constexpr float kMaxDRCaloTowers_=0.5;
public:
  SHEventHelper();
  template<typename T>
  void setup(const edm::ParameterSet& conf,edm::ConsumesCollector && cc,T& module){   
    minEtToPromoteSC_ = conf.getParameter<double>("minEtToPromoteSC");
    minEtToSaveEle_ = conf.getParameter<double>("minEtToSaveEle");
    eventWeight_ = conf.getParameter<double>("sampleWeight");
    datasetCode_ = conf.getParameter<int>("datasetCode");    
    applyMuonId_ = conf.getParameter<bool>("applyMuonId");
    fillFromGsfEle_ = conf.getParameter<bool>("fillFromGsfEle");
    trigSumMaker_.setup(cc);
    
    isMC_=datasetCode_!=0;
    branches_.setup(conf);
    if(branches_.addHLTDebug){
      getterOfProducts_ = edm::GetterOfProducts<reco::RecoEcalCandidateIsolationMap>(edm::ProcessMatch("*"), &module);
      module.callWhenNewProductsRegistered(getterOfProducts_);
    }
  }
  
  void setupRun(const edm::Run& run,const edm::EventSetup& setup);

  //the two modifiers
  void setDatasetCode(int datasetCode){datasetCode_=datasetCode;}
  void setEventWeight(float weight){eventWeight_=weight;}

  //the main master function which calls all the others
  void makeSHEvent(const heep::Event & heepEvent, SHEvent& shEvent)const;
 
  void addEventPara(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addElectrons(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const edm::Ptr<reco::GsfElectron>& gsfEle)const;
  void addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const reco::Photon& photon)const;
  void addSuperClusters(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addPreShowerClusters(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addCaloHits(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addCaloTowers(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addEcalHits(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addEcalWeightsHits(const heep::Event& heepEvent, SHEvent& shEvent)const;
  void addHcalHits(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addJets(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addIsolTrks(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addMet(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addMCParticles(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addMuons(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addTrigInfo(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addPUInfo(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addGenInfo(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addPFCands(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addPFClusters(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addGSFixInfo(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addJetMETExtra(const heep::Event& heepEvent,SHEvent& shEvent)const;

  static std::vector<const EcalRecHit*> getGainSwitchedHits(const EcalRecHitCollection& hits);
 
  static int getVertexNr(const reco::TrackBase& track,const std::vector<reco::Vertex>& vertices);
  static int getVertexNrClosestZ(const reco::TrackBase& track,const std::vector<reco::Vertex>& vertices);

  static bool passMuonId(const reco::Muon& muon,const heep::Event& heepEvent);
  static void fillRecHitClusterMap(const reco::SuperCluster& superClus,SHEvent& shEvent);
private:
  //the hashing functions for vector positions
  int ecalHitHash_(const DetId detId)const;
  int hcalHitHash_(const DetId detId)const; 
  
  //makes the ecal and hcalHitVec the correct size, puts the correct detId in the correct position and -999.s the energies
  void initEcalHitVec_()const;
  void initHcalHitVec_()const;
  
  static uint32_t getEcalFlagBits_(const EcalRecHit& hit);//because a simple accessor to the bit was too much to ask
  static int getTrkQuality_(const reco::Track& trk);//likewise..
  void addIsolTrksFromTrks_(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addIsolTrksFromCands_(const heep::Event& heepEvent,SHEvent& shEvent)const;
  void addIsolTrksFromCands_(const std::vector<pat::PackedCandidate>& cands,
			     const heep::Event& heepEvent,SHEvent& shEvent,bool rejectEles)const;
  
  void fillPFClustersECAL_(const SHEvent* event,double maxDR,SHPFClusterContainer& shPFClusters,const std::vector<reco::PFCluster>& pfClusters,const std::vector<reco::SuperCluster>& scEB,const std::vector<reco::SuperCluster>& scEE)const;
  void fillPFClustersHCAL_(const SHEvent* event,double maxDR,SHPFClusterContainer& shPFClusters,const std::vector<reco::PFCluster>& pfClusters)const;
  int getSCSeedCrysId_(uint pfSeedId,const std::vector<reco::SuperCluster>& superClusters)const;
  void fixTrkIsols_(const heep::Event& heepEvent,const edm::Ptr<reco::GsfElectron>& gsfEle,SHElectron& shEle)const;
  void setCutCode_(const heep::Event& heepEvent,const edm::Ptr<reco::GsfElectron>& gsfEle,SHElectron& shEle)const;
  void addUserData_(const reco::GsfElectron* gsfEle,SHElectron& ele)const;
  void addUserData_(const reco::Photon* pho,SHElectron& ele)const;
  void setNrSatCrysIn5x5_(const heep::Event& heepEvent,SHElectron& shEle)const;
  bool isNearEle_(float eta,float phi,const SHEvent& shEvent,const float maxDR)const;
  void fillEcalHitVec_(const heep::Event& heepEvent,const EcalRecHitCollection& hitColl,const SHEvent& event)const;
  float getEcalRecHitAmplitude(const heep::Event& heepEvent,int detId,float calibNrgy)const;
  bool passCaloHitFilter_(int hitId,const SHEvent& shEvent,const float maxDR)const;
  void setRecHit(SHCaloHit& shHit,const EcalRecHit& hit,const heep::Event& heepEvent)const;
  static bool isEcalBarrel_(const DetId& id);
  static bool isEcalEndcap_(const DetId& id);
  static bool isEcalBarrel_(const reco::SuperCluster& sc){return isEcalBarrel_(sc.seed()->seed());}
  static bool isEcalEndcap_(const reco::SuperCluster& sc){return isEcalEndcap_(sc.seed()->seed());}
  static void fillRecHitClusterMap_(const reco::CaloCluster& clus,SHEvent& shEvent);
  template<typename T> static size_t matchToEle(const reco::SuperCluster& sc,const T& eles){
    for(size_t eleNr=0;eleNr<eles.size();eleNr++){
      if(eles[eleNr].superCluster()->seed()->seed()==sc.seed()->seed()) return eleNr;
    }
    return eles.size();
  }
  SHIsolTrack createSHIsolTrack_(const pat::PackedCandidate& cand,
				 const heep::Event& heepEvent)const;
  const reco::GsfElectron* matchTrkToEle_(const reco::TrackBase& cand,
					  const heep::Event& heepEvent)const;
  
  
  static const reco::Photon* getPhoMatch_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::Event& heepEvent);
  static const reco::GsfElectron* getOldEleMatch_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::Event& heepEvent);
  static const reco::Photon* getOldPhoMatch_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::Event& heepEvent);
  static const reco::Photon* getOldPhoMatch_(const reco::Photon& pho,const heep::Event& heepEvent);
  template<typename T>
  static bool hasGSIn5x5_(edm::Handle<edm::View<T> > handle,
			  const EcalRecHitCollection& recHits,const CaloTopology& caloTopo);
};


template<typename T>
bool SHEventHelper::
hasGSIn5x5_(edm::Handle<edm::View<T> > handle,const EcalRecHitCollection& recHits,const CaloTopology& caloTopo)
{ 
  // if(handle.isValid()){ 
  //   for(auto& obj : *handle){
  //     if(GainSwitchTools::hasEBGainSwitchIn5x5(*obj.superCluster(),&recHits,&caloTopo) ) return true;
  //   }
  // }
  return false;
}
#endif
