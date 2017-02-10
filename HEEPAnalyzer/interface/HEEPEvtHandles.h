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
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/PreshowerCluster.h"
#include "DataFormats/EgammaReco/interface/PreshowerClusterFwd.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"

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
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h" 
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h" 
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"


namespace heep {
  struct EvtHandles { 
  public:   
    edm::ESHandle<CaloGeometry> caloGeom;
    edm::ESHandle<CaloTopology> caloTopology;
    edm::Handle<EcalRecHitCollection> ebRecHits;
    edm::Handle<EcalRecHitCollection> eeRecHits;
    edm::Handle<EcalRecHitCollection> ebReducedRecHits;
    edm::Handle<EcalRecHitCollection> eeReducedRecHits;
    edm::Handle<EcalRecHitCollection> ebWeightsRecHits;
    edm::Handle<EcalRecHitCollection> eeWeightsRecHits;    
    edm::Handle<HBHERecHitCollection> hbheRecHits;
    edm::Handle<CaloTowerCollection> caloTowers;
    edm::Handle<edm::View<pat::Muon> > muon;
    edm::Handle<edm::View<pat::Jet> > jet;
    edm::Handle<edm::View<pat::Electron> > electron;
    edm::Handle<edm::View<pat::MET> > met;
    edm::Handle<edm::View<pat::Photon> > pho;
    edm::Handle<edm::View<pat::Tau> > tau;
    edm::Handle<std::vector<pat::PackedCandidate> > packedPFCand;
    edm::Handle<std::vector<pat::PackedCandidate> > lostTrack;

    edm::Handle<reco::PFCandidateCollection> pfCandidate;
    edm::Handle<reco::SuperClusterCollection> superClusEB;
    edm::Handle<reco::SuperClusterCollection> superClusEE;  
    edm::Handle<reco::PreshowerClusterCollection> preShowerClusX;
    edm::Handle<reco::PreshowerClusterCollection> preShowerClusY; 
    edm::Handle<edm::View<reco::GsfElectron> > gsfEle; 
    edm::Handle<edm::View<reco::GsfElectron> > oldGsfEle; 
    edm::Handle<edm::View<reco::Photon> > recoPho;
    edm::Handle<reco::TrackCollection> ctfTrack;
    edm::Handle<reco::GenParticleCollection> genParticle;
    edm::Handle<std::vector<pat::PackedGenParticle> > packedGenParticle;
    edm::Handle<trigger::TriggerEvent> trigEvent;
    edm::Handle<std::vector<pat::TriggerObjectStandAlone> > patTrigObjs;
    edm::Handle<trigger::TriggerObjectCollection> trigObjColl;
    edm::Handle<edm::TriggerResults> trigResults;
    edm::Handle<GenEventInfoProduct> genEventInfo;
    edm::Handle<std::vector<PileupSummaryInfo> > pileUpMCInfo;
    edm::Handle<L1GlobalTriggerReadoutRecord> l1Record;
    edm::Handle<reco::VertexCollection> vertices;
    edm::Handle<reco::BeamSpot> beamSpot;
    edm::Handle<double> eleRhoCorr;
    edm::Handle<double> eleRhoCorr2012;
    edm::Handle<bool> ecalLaserFilter;
    edm::ESHandle<L1GtTriggerMenu> l1Menu;
    
    edm::Handle<edm::ValueMap<double> > pfChargedIsoValEleMap;
    edm::Handle<edm::ValueMap<double> > pfPhotonIsoValEleMap;
    edm::Handle<edm::ValueMap<double> > pfNeutralIsoValEleMap;
    edm::Handle<edm::ValueMap<float> > eleIsolPtTrksValueMap;

    edm::Handle<std::vector<reco::PFCluster> > pfClustersHCAL;
    edm::Handle<std::vector<reco::PFCluster> > pfClustersECAL;

    edm::Handle<edm::ValueMap<std::vector<reco::PFCandidateRef> > > gsfEleToPFCandMap;
    
    edm::Handle<edm::ValueMap<bool> > heepIDVID;
    edm::Handle<edm::ValueMap<unsigned int> >  heepIDVIDBits;
    edm::Handle<reco::RecoEcalCandidateCollection>  egHLTCands;
    edm::Handle<reco::RecoEcalCandidateCollection>  egHLTCandsUnseeded;
    
    edm::Handle<LHEEventProduct> lheEvent;
    edm::Handle<GenEventInfoProduct> genEvtInfo;
    edm::ESHandle<MagneticField> bField;
    edm::ESHandle<TrackerGeometry> trackGeom;

    //calibrations for the ECAL
    edm::ESHandle<EcalADCToGeVConstant> ecalADCToGeV;
    edm::ESHandle<EcalLaserDbService> ecalLaser;
    edm::ESHandle<EcalIntercalibConstants> ecalInterCalib;

    edm::Handle<bool> gsFixDupECALClusters;
    edm::Handle<DetIdCollection> gsFixHitsNotReplaced;
    edm::Handle<EcalRecHitCollection> gsFixOrgReducedEGEBHits;
    edm::Handle<edm::View<pat::MET> > gsFixMETOrg;
    edm::Handle<edm::View<pat::MET> > gsFixMETEGClean;
  };
}

#endif
