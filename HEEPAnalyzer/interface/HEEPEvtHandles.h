#ifndef SHARPER_HEEPANALYZER_HEEPEVTHANDLES
#define SHARPER_HEEPANALYZER_HEEPEVTHANDLES

//struct: heep::EvtHandles (heep::Event Handles)
//Description: a simple struct which stores handles which are usefull to the heep group
//
//
//Implimentation: 
//  only handles of objects allowed to be stored in the event are allowed here
//  no .cc file as unnecessary
//
  
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"

#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h" 
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "MagneticField/Engine/interface/MagneticField.h"



namespace heep {
  struct EvtHandles { 
  public:   
    edm::ESHandle<CaloGeometry> caloGeom;
    edm::ESHandle<CaloTopology> caloTopology;
    edm::Handle<EcalRecHitCollection> ebRecHits;
    edm::Handle<EcalRecHitCollection> eeRecHits;
    edm::Handle<EcalRecHitCollection> ebReducedRecHits;
    edm::Handle<EcalRecHitCollection> eeReducedRecHits;
    edm::Handle<HBHERecHitCollection> hbheRecHits;
    edm::Handle<CaloTowerCollection> caloTowers;
    edm::Handle<edm::View<pat::Muon> > muon;
    edm::Handle<edm::View<pat::Jet> > jet;
    edm::Handle<edm::View<pat::Electron> > electron;
    edm::Handle<edm::View<pat::MET> > met;
    edm::Handle<edm::View<pat::Photon> > pho;
    edm::Handle<edm::View<pat::Tau> > tau;

    edm::Handle<reco::SuperClusterCollection> superClusEB;
    edm::Handle<reco::SuperClusterCollection> superClusEE;  
    edm::Handle<reco::GsfElectronCollection> gsfEle; 
    edm::Handle<reco::PhotonCollection> recoPho;
    edm::Handle<reco::TrackCollection> ctfTrack;
    edm::Handle<reco::GenParticleCollection> genParticle;
    edm::Handle<trigger::TriggerEvent> trigEvent;
    edm::Handle<trigger::TriggerObjectCollection> trigObjColl;
    edm::Handle<edm::TriggerResults> trigResults;
    edm::Handle<GenEventInfoProduct> genEventInfo;
    edm::Handle<std::vector<PileupSummaryInfo> > pileUpMCInfo;
    edm::Handle<L1GlobalTriggerReadoutRecord> l1Record;
    edm::Handle<l1extra::L1EmParticleCollection> l1EmNonIso;
    edm::Handle<l1extra::L1EmParticleCollection> l1EmIso;
    edm::Handle<reco::VertexCollection> vertices;
    edm::Handle<reco::BeamSpot> beamSpot;
    edm::Handle<double> eleRhoCorr;
    edm::ESHandle<L1GtTriggerMenu> l1Menu;
    

    edm::ESHandle<MagneticField> bField;
    edm::ESHandle<TrackerGeometry> trackGeom;
  };
}

#endif
