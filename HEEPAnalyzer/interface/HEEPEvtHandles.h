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
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"


namespace heep {
  struct EvtHandles { 
  public:   
    edm::ESHandle<CaloGeometry> caloGeom;
    edm::ESHandle<CaloTopology> caloTopology;
    edm::Handle<EcalRecHitCollection> ebRecHits;
    edm::Handle<EcalRecHitCollection> eeRecHits;
   
    edm::Handle<edm::View<pat::Muon> > muon;
    edm::Handle<edm::View<pat::Jet> > jet;
    edm::Handle<edm::View<pat::Electron> > electron;
    edm::Handle<edm::View<pat::MET> > met;
    edm::Handle<edm::View<pat::Photon> > pho;
    edm::Handle<edm::View<pat::Tau> > tau;

  };
}

#endif
