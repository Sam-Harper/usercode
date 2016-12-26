#include "PhysicsTools/PatAlgos/plugins/ModifiedObjectProducer.h"
    
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
     
typedef pat::ModifiedObjectProducer<reco::GsfElectron> ModifiedGsfElectronProducer;
typedef pat::ModifiedObjectProducer<reco::Photon> ModifiedRecoPhotonProducer;
    
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ModifiedGsfElectronProducer);
DEFINE_FWK_MODULE(ModifiedRecoPhotonProducer);
