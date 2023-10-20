#ifndef SHARPER_HEEPANALYZER_HEEPGSFELEEXTRAFILLER_H
#define SHARPER_HEEPANALYZER_HEEPGSFELEEXTRAFILLER_H


#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtra.h"

namespace heep {
  class GsfEleExtraFiller {
  private:
    edm::EDGetTokenT<reco::VertexCollection> verticesToken_;
    edm::EDGetTokenT<double> rhoToken_;
    edm::EDGetTokenT<edm::ValueMap<float> > trkIsoNoJetCoreToken_;
    edm::EDGetTokenT<edm::ValueMap<int> > nrSatCrysIn5x5Token_;
    
    edm::Handle<reco::VertexCollection> verticesHandle_;
    edm::Handle<double> rhoHandle_;
    edm::Handle<edm::ValueMap<float> > trkIsoNoJetCoreHandle_;
    edm::Handle<edm::ValueMap<int> > nrSatCrysIn5x5Handle_;

    //to get number of saturated crystals
    edm::EDGetTokenT<EcalRecHitCollection> ebRecHitToken_;
    edm::EDGetTokenT<EcalRecHitCollection> eeRecHitToken_;
    edm::Handle<EcalRecHitCollection> ebRecHitHandle_;
    edm::Handle<EcalRecHitCollection> eeRecHitHandle_;
    edm::ESGetToken<CaloTopology,CaloTopologyRecord> caloTopoToken_;
    edm::ESHandle<CaloTopology> caloTopoHandle_;
    //trk isol stuff
    edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
    edm::EDGetTokenT<reco::TrackCollection> trkToken_;
    edm::Handle<reco::BeamSpot> beamSpotHandle_;
    edm::Handle<reco::TrackCollection> trkHandle_;
    struct TrkIsoParam {
      double extRadius;
      double intRadiusBarrel;
      double intRadiusEndcap;
      double stripBarrel;
      double stripEndcap;
      double ptMin;
      double maxVtxDist;
      double drb;
      TrkIsoParam(){}
      TrkIsoParam(const edm::ParameterSet& iPara);
    };
    const TrkIsoParam trkIsoParam_;
    
  public:
    GsfEleExtraFiller(){}
    GsfEleExtraFiller(const edm::ParameterSet& iPara,edm::ConsumesCollector&& iC);
    void setup(const edm::ParameterSet& iPara,edm::ConsumesCollector& iC);
    void getEvtContent(const edm::Event& iEvent,const edm::EventSetup& iSetup);
    void operator()(const edm::Ptr<reco::GsfElectron>& ele,heep::GsfEleExtra& eleExtra);
    heep::GsfEleExtra operator()(const edm::Ptr<reco::GsfElectron>& ele);

  private:
    float getTrkIsoNoJetCore_(const edm::Ptr<reco::GsfElectron>& ele);
    float getNrSatCrysIn5x5_(const edm::Ptr<reco::GsfElectron>& ele);
    
    template <typename T>
    static void setToken(edm::EDGetTokenT<T>& token,edm::ConsumesCollector& iC,
			 const edm::ParameterSet& iPara,const std::string& name){
      token = iC.consumes<T>(iPara.getParameter<edm::InputTag>(name));
    }
    
    
  };
}

#endif
