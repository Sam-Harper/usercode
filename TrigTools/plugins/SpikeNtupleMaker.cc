#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "TTree.h"
#include "TLorentzVector.h"

  struct P4Struct {
    float et,energy,pt,eta,phi;
    P4Struct(){}
    P4Struct(const TLorentzVector& p4){fill(p4);}
    void fill(const TLorentzVector&p4){
      et=p4.Et();pt=p4.Pt();energy=p4.E();eta=p4.Eta();phi=p4.Phi();
    } 
    P4Struct(const math::XYZTLorentzVector & p4){fill(p4);}
    void fill(const math::XYZTLorentzVector&p4){
      et=p4.Et();pt=p4.Pt();energy=p4.E();eta=p4.Eta();phi=p4.Phi();
    }
    P4Struct& operator=(float val){et=val;energy=val;eta=val;phi=val;return *this;}
    P4Struct& operator=(int val){et=val;energy=val;eta=val;phi=val;return *this;}
    P4Struct& operator=(const TLorentzVector&p4){fill(p4);return *this;} 
    P4Struct& operator=(const math::XYZTLorentzVector&p4){fill(p4);return *this;}
    static std::string contents(){return "et/F:energy:pt:eta:phi";}
  };


struct SpikeData {
  float et,energy,eta,phi,iEtaOrIX,iPhiOrIY,zSide,subDet,e4e1,e6e2,e1;
  static std::string contents(){return "et/F:energy:eta:phi:iEtaOrIX:iPhiOrIY:zSide:subDet:e4e1:e6e2:e1";}
  void fill(const reco::RecoEcalCandidate& cand,const EcalRecHitCollection& recHits);
};

class SpikeFuncs { 
  SpikeFuncs()=delete;
  ~SpikeFuncs()=delete;

public:  
  static const std::vector<DetId> neighbours(const DetId& id);
  static float e4e1(const DetId& id,const EcalRecHitCollection& recHits);
  static float e6e2(const DetId& id,const EcalRecHitCollection& recHits);
  static float recHitE(const DetId& id,const EcalRecHitCollection& recHits);

};


class SpikeNtupleMaker : public edm::EDAnalyzer {
public:
  explicit SpikeNtupleMaker(const edm::ParameterSet&);
  ~SpikeNtupleMaker();
  
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  static const reco::GenParticle*  getMatchedMonopole(const float eta,const float phi,edm::Handle<std::vector<reco::GenParticle> > genParts,float& bestDeltaR);

private:
  // ----------member data ---------------------------
  
  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> recoEcalCandToken_;
  edm::EDGetTokenT<EcalRecHitCollection>  ecalRecHitEBToken_;
  edm::EDGetTokenT<EcalRecHitCollection>  ecalRecHitEEToken_;
  edm::EDGetTokenT<std::vector<reco::GenParticle> > genParticleToken_;
 
  SpikeData spikeData_;
  P4Struct genP4_;
  float deltaR_;
  TTree* ntupTree_;//not owned by us, ROOT owns it
};

SpikeNtupleMaker::SpikeNtupleMaker(const edm::ParameterSet& conf):ntupTree_(nullptr)
{
  recoEcalCandToken_ = consumes<reco::RecoEcalCandidateCollection>(conf.getParameter<edm::InputTag>("recoEcalCands"));
  ecalRecHitEBToken_ = consumes<EcalRecHitCollection>(conf.getParameter< edm::InputTag > ("ecalRecHitEB"));
  ecalRecHitEEToken_ = consumes<EcalRecHitCollection>(conf.getParameter< edm::InputTag > ("ecalRecHitEE"));
  genParticleToken_ = consumes<std::vector<reco::GenParticle> > (edm::InputTag("genParticles"));
}

SpikeNtupleMaker::~SpikeNtupleMaker(){}

void SpikeNtupleMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  ntupTree_ = new TTree("spikeTree","Spike Tree");
  ntupTree_->Branch("eg1",&spikeData_,SpikeData::contents().c_str());
  ntupTree_->Branch("truth",&genP4_,P4Struct::contents().c_str());
  ntupTree_->Branch("dRTruth",&deltaR_,"dRTruth/F");
}


void SpikeNtupleMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<reco::RecoEcalCandidateCollection> recoEcalCandHandle;
  iEvent.getByToken(recoEcalCandToken_,recoEcalCandHandle);

  edm::Handle<EcalRecHitCollection> ecalRecHitEBHandle;
  iEvent.getByToken(ecalRecHitEBToken_,ecalRecHitEBHandle);
  
  edm::Handle<EcalRecHitCollection> ecalRecHitEEHandle;
  iEvent.getByToken(ecalRecHitEEToken_,ecalRecHitEEHandle);
  
  edm::Handle<std::vector<reco::GenParticle> > genParticleHandle;
  iEvent.getByToken(genParticleToken_,genParticleHandle);
  
  for(size_t partNr=0;partNr<genParticleHandle->size();partNr++){
    const reco::GenParticle& genPart = (*genParticleHandle)[partNr];
    std::cout <<"part "<<partNr<<" pdgId "<<genPart.pdgId()<<" status "<<genPart.status()<<" et "<<genPart.et()<<" energy "<<genPart.energy()<<std::endl;
  }
  
  for(size_t candNr=0;candNr<recoEcalCandHandle->size();candNr++){
    const reco::RecoEcalCandidate& cand = (*recoEcalCandHandle)[candNr];
    bool isEB = cand.superCluster()->seed()->seed().subdetId()==EcalBarrel;
    
    const reco::GenParticle* genPart = getMatchedMonopole(cand.eta(),cand.phi(),genParticleHandle,deltaR_);
    if(genPart) genP4_.fill(genPart->p4());
    else genP4_=-999;
    spikeData_.fill(cand,isEB ? *ecalRecHitEBHandle.product() : *ecalRecHitEEHandle.product());
    ntupTree_->Fill();
  }
}

const reco::GenParticle*  SpikeNtupleMaker::getMatchedMonopole(const float eta,const float phi,edm::Handle<std::vector<reco::GenParticle> > genParts,float& bestDeltaR)
{
  bestDeltaR=999;
  const reco::GenParticle* bestPart=nullptr;

  if(!genParts.isValid()) return bestPart;
  
  for(size_t partNr=0;partNr<genParts->size();partNr++){
    const reco::GenParticle& genPart = (*genParts)[partNr];
    if(std::abs(genPart.pdgId())==4110000 && genPart.status()==1){
      float dR = reco::deltaR(eta,phi,genPart.eta(),genPart.phi());
      if(dR<bestDeltaR){
	bestPart=&genPart;
	bestDeltaR=dR;
      }
    }
  }  
  
  return bestPart;
}
void SpikeData::fill(const reco::RecoEcalCandidate& cand,const EcalRecHitCollection& recHits)
{
  et = cand.et();
  energy = cand.energy();
  eta = cand.eta();
  phi = cand.phi();
  DetId seedDetId =  cand.superCluster()->seed()->seed();
  if(seedDetId.subdetId()==EcalBarrel){
    EBDetId ebId(seedDetId);
    iEtaOrIX = ebId.ieta();
    iPhiOrIY = ebId.iphi();
    zSide = eta < 0 ? -1 : 1;
    subDet = 0;
  }else if(seedDetId.subdetId()==EcalEndcap){
    EEDetId eeId(seedDetId);
    iEtaOrIX = eeId.ix();
    iPhiOrIY = eeId.iy();
    zSide = eta < 0 ? -1 : 1;
    subDet = 1;
  }else{
    iEtaOrIX = 0;
    iPhiOrIY = 0;
    zSide = 0;
    subDet = -1;
  }

  e4e1 = SpikeFuncs::e4e1(seedDetId,recHits);
  e6e2 = SpikeFuncs::e6e2(seedDetId,recHits);


}
 
//solten from EcalCleaningAlgo (and all the related functions)
const std::vector<DetId> SpikeFuncs::neighbours(const DetId& id)
{
  std::vector<DetId> ret;
  
  if ( id.subdetId() == EcalBarrel) {
    
    ret.push_back( EBDetId::offsetBy( id,  1, 0 ));
    ret.push_back( EBDetId::offsetBy( id, -1, 0 ));
    ret.push_back( EBDetId::offsetBy( id,  0, 1 ));
    ret.push_back( EBDetId::offsetBy( id,  0,-1 ));
  }
  // nobody understands what polymorphism is for, sgrunt !
  else  if (id.subdetId() == EcalEndcap) {
    ret.push_back( EEDetId::offsetBy( id,  1, 0 ));
    ret.push_back( EEDetId::offsetBy( id, -1, 0 ));
    ret.push_back( EEDetId::offsetBy( id,  0, 1 ));
    ret.push_back( EEDetId::offsetBy( id,  0,-1 ));
    
  }
  return ret;

}

    
    
float SpikeFuncs::e4e1(const DetId& id, 
		      const EcalRecHitCollection& recHits){

 
  float s4 = 0;
  float e1 = recHitE( id, recHits);
  
  
  if ( e1 == 0 ) return 0;
  const std::vector<DetId>& neighs =  neighbours(id);
  for (size_t i=0; i<neighs.size(); ++i)
    // avoid hits out of time when making s4
    s4+=recHitE(neighs[i],recHits);
  
  return s4 / e1;
  
 
}

float SpikeFuncs::e6e2(const DetId& id, 
		      const EcalRecHitCollection& recHits){

    float s4_1 = 0;
    float s4_2 = 0;
    float e1 = recHitE(id, recHits);

    float maxene=0;
    DetId maxid;

    if ( e1 == 0 ) return 0;

    const std::vector<DetId>& neighs =  neighbours(id);

    // find the most energetic neighbour ignoring time info
    for (size_t i=0; i<neighs.size(); ++i){
      float ene = recHitE(neighs[i],recHits);
      if (ene>maxene)  {
	maxene=ene;
	maxid = neighs[i];
      }
    }
    float e2=maxene;
    s4_1 = e4e1(id,recHits)* e1;
    s4_2 = e4e1(maxid,recHits)* e2;
    return (s4_1 + s4_2) / (e1+e2) -1. ;
}

float SpikeFuncs::recHitE( const DetId& id, 
			  const EcalRecHitCollection &recHits)
{
  if ( id.rawId() == 0 ) return 0;
  float threshold = id.subdetId() == EcalEndcap ? 0.08 : 0.3;
  EcalRecHitCollection::const_iterator it = recHits.find( id );
  if ( it != recHits.end() ){
    float ene= (*it).energy();
    // ignore hits below threshold
    if (ene < threshold) return 0;
    // else return the energy of this hit
    return ene;
  }
  return 0;
}

//define this as a plug-in
DEFINE_FWK_MODULE(SpikeNtupleMaker);
