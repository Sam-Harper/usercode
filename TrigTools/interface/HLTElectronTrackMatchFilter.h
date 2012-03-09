#ifndef HLTElectronTrackMatchFilter_h
#define HLTElectronTrackMatchFilter_h

/** \class HLTElectronTrackMatchFilter
 *
 *  \author Monica Vazquez Acosta (CERN)
 *
 */

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "TrackingTools/GsfTools/interface/MultiTrajectoryStateTransform.h"
#include "TrackingTools/GsfTools/interface/MultiTrajectoryStateMode.h"
#include "RecoEgamma/EgammaElectronAlgos/interface/ElectronUtilities.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
//#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
//
// class decleration
//
#include "TTree.h"
#include "TFile.h"


class HLTElectronTrackMatchFilter : public HLTFilter {
 
public:
  //a class designed to be a simple version of reco::GsfElectron
  //we cant use reco::GsfElectron as it requires calculating a lot of things we dont need
  //it is designed to be template compatible with reco::GsfElectron
  typedef std::pair<const reco::GsfTrack*,float> GsfTrkEpPair;
  
  struct isInnerMost
   {
     const edm::ESHandle<TrackerGeometry> trackerHandle_ ;
     isInnerMost(const edm::ESHandle<TrackerGeometry> geom ) : trackerHandle_(geom) {}
     bool operator()(const GsfTrkEpPair& e1, const GsfTrkEpPair& e2);
  } ;
private:
  class EventSetupData {
   
   
    unsigned long long cacheIDTDGeom_;
    unsigned long long cacheIDMagField_;
    
    edm::ESHandle<MagneticField> magField_;
    edm::ESHandle<TrackerGeometry> trackerHandle_;
    
    MultiTrajectoryStateMode mtsMode_; 
    const MultiTrajectoryStateTransform * mtsTransform_; //we own it
  

  public:
    EventSetupData():cacheIDTDGeom_(0),cacheIDMagField_(0),mtsTransform_(0){}
    EventSetupData(const EventSetupData& rhs);
    ~EventSetupData(){delete mtsTransform_;}
    EventSetupData* operator=(const EventSetupData& rhs);
    
    void setup(const edm::EventSetup& iSetup);

    GlobalPoint extrapolateTrackPosToPoint(const reco::GsfTrack& gsfTrack,const GlobalPoint& pointToExtrapTo);
    GlobalVector extrapolateTrackMomToPoint(const reco::GsfTrack& gsfTrack,const GlobalPoint& pointToExtrapTo);

    edm::ESHandle<TrackerGeometry> trackerGeomHandle()const{return trackerHandle_;}
    const MultiTrajectoryStateTransform * mtsTransform()const{return mtsTransform_;}
    const MultiTrajectoryStateMode* mtsMode()const{return &mtsMode_;}
  };

public:
  explicit HLTElectronTrackMatchFilter(const edm::ParameterSet&);
  ~HLTElectronTrackMatchFilter();
  virtual bool hltFilter(edm::Event&, const edm::EventSetup&, trigger::TriggerFilterObjectWithRefs & filterproduct);
private:


  bool matchedTrack(const reco::GsfTrackCollection& gsfTracks,const reco::SuperClusterRef superClusRef,const reco::BeamSpot& beamSpot);
  edm::InputTag candTag_;     // input tag identifying product contains filtered egammas
  
  edm::InputTag l1IsoGsfTracksTag_; 
  edm::InputTag l1NonIsoGsfTracksTag_;
  edm::InputTag beamSpotTag_;
  
  int    ncandcut_;           // number of electrons required
  
  bool doIsolated_;
  bool   store_;
  edm::InputTag l1IsoCollTag_; 
  edm::InputTag l1NonIsoCollTag_;
  
  float dPhiMaxEB_;
  float dEtaMaxEB_;
  float dPhiMaxEE_;
  float dEtaMaxEE_;
  edm::InputTag debugDEtaInputTag_;
  edm::InputTag debugDPhiInputTag_; 
  EventSetupData eventSetupData_;

  //my debugging info
  struct EleDataStruct {
    float nrgy,et,detEta,detPhi;
    float trkPt,trkP,trkEta,trkPhi,q;
    float trkEtaAtSC,trkPhiAtSC;
    float dPhi,dEta;
    float nrMissHits;
    float nrTracks;
    float matteoDEta,matteoDPhi,minDEta,minDPhi; //temp debugging variables and not affected by fill(), should go last
    float pass;
    void fill(const reco::SuperCluster& superClus,const reco::GsfTrack& trk,const GlobalPoint& trkPosAtSC,float iDEta,float iDPhi,float iNrTracks); 
    void fill(const reco::SuperCluster& superClus); 
    static std::string contents(){return "nrgy/F:et:detEta:detPhi:trkPt:trkP:trkEta:trkPhi:q:trkEtaAtSC:trkPhiAtSC:dPhi:dEta:nrMissHits:nrTracks:matteoDEta:matteoDPhi:minDEta:minDPhi:pass";}
  };
  struct EvtInfoStruct {
    int runnr,lumiSec,eventnr,bx;
    void fill(const edm::Event& event);
    static std::string contents(){return "runnr/I:lumiSec:eventnr:bx";}
  };
  struct PosStruct {
    float x,y,z;
    void fill(const math::XYZPoint& point){x=point.x();y=point.y();z=point.z();}  
    static std::string contents(){return "x/F:y:z";}
  };
   
  EleDataStruct eleData_;
  EvtInfoStruct evtInfo_;
  PosStruct beamSpotPos_;
  int trkNr_;
  TTree* tree_;
  TFile* file_;
  
  
};

#endif //HLTElectronTrackMatchFilter_h


