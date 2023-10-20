#ifndef HOverEComparer_h
#define HOverEComparer_h

/** \class HLTElectronTrackMatchFilter
 *
 *  \author Monica Vazquez Acosta (CERN)
 *
 */

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
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


class HOverEComparer : public edm::one::EDAnalyzer<> {
 
public:
  

public:
  explicit HOverEComparer(const edm::ParameterSet&);
  ~HOverEComparer();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:


  bool matchedTrack(const reco::GsfTrackCollection& gsfTracks,const reco::SuperClusterRef superClusRef,const reco::BeamSpot& beamSpot);
  edm::InputTag candTag_;     // input tag identifying product contains filtered egammas
 
  
  edm::InputTag oldHadTag_;
  edm::InputTag newHadTag_;

  //my debugging info
  struct EleDataStruct {
    float nrgy,et,detEta,detPhi;
    float newHad,oldHad;
   
    void fill(const reco::SuperCluster& superClus,float iNewHad,float iOldHad);
    
    static std::string contents(){return "nrgy/F:et:detEta:detPhi:newHad:oldHad";}
  };
  struct EvtInfoStruct {
    int runnr,lumiSec,eventnr,bx;
    void fill(const edm::Event& event);
    static std::string contents(){return "runnr/I:lumiSec:eventnr:bx";}
  };
 
  EleDataStruct eleData_;
  EvtInfoStruct evtInfo_;
  int eleNr_;
  TTree* tree_;
  TFile* file_;
  
  
};

#endif //HOverEComparer_h


