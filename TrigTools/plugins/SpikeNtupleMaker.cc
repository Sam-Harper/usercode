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
  P4Struct& operator=(double val){et=val;energy=val;eta=val;phi=val;return *this;}
  P4Struct& operator=(int val){et=val;energy=val;eta=val;phi=val;return *this;}
  P4Struct& operator=(const TLorentzVector&p4){fill(p4);return *this;} 
  P4Struct& operator=(const math::XYZTLorentzVector&p4){fill(p4);return *this;}
  static std::string contents(){return "et/F:energy:pt:eta:phi";}
};




struct SpikeData {
  float et,energy,eta,phi,iEtaOrIX,iPhiOrIY,zSide,subDet,e4e1,e6e2,e1,passL1;
  static std::string contents(){return "et/F:energy:eta:phi:iEtaOrIX:iPhiOrIY:zSide:subDet:e4e1:e6e2:e1:passL1";}
  void fill(const reco::RecoEcalCandidate& cand,const EcalRecHitCollection& recHits,bool l1Matched);
  void clear(){et=energy=eta=phi=iEtaOrIX=iPhiOrIY=zSide=subDet=e4e1=e6e2=e1=passL1=-999.;}
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

struct EvtInfoStruct {
  int runnr,lumiSec,eventnr,bx;
  void fill(const edm::Event& event);

  static std::string contents(){return "runnr/I:lumiSec:eventnr:bx";}
};

void EvtInfoStruct::fill(const edm::Event& event)
{
  runnr = event.id().run();
  lumiSec = event.luminosityBlock();
  eventnr = event.id().event();
  bx = event.bunchCrossing();
}

class SpikeNtupleMaker : public edm::EDAnalyzer {
public:
  explicit SpikeNtupleMaker(const edm::ParameterSet&);
  ~SpikeNtupleMaker();
  
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  static const reco::GenParticle*  getMatchedMonopole(const float eta,const float phi,edm::Handle<std::vector<reco::GenParticle> > genParts,float& bestDeltaR);
  const reco::GenParticle* getPart(edm::Handle<std::vector<reco::GenParticle> > genParts,int partId);
  const reco::RecoEcalCandidate* matchClusInCollection(const math::XYZTLorentzVector& p4,edm::Handle<std::vector<reco::RecoEcalCandidate> >& collection,const float bestDeltaR,float bestRelEtDiff);
private:
  // ----------member data ---------------------------
  
  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> recoEcalCandToken_;  
  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> recoEcalCandL1SeededToken_;
  edm::EDGetTokenT<EcalRecHitCollection>  ecalRecHitEBToken_;
  edm::EDGetTokenT<EcalRecHitCollection>  ecalRecHitEEToken_;
  edm::EDGetTokenT<std::vector<reco::GenParticle> > genParticleToken_;
 
  EvtInfoStruct evtData_;
  int nrEG_;
  std::vector<SpikeData> egData_; 
  P4Struct mp1P4_,mp2P4_;
  TTree* ntupTree_;//not owned by us, ROOT owns it
};

SpikeNtupleMaker::SpikeNtupleMaker(const edm::ParameterSet& conf):egData_(4),ntupTree_(nullptr)
{
  recoEcalCandToken_ = consumes<reco::RecoEcalCandidateCollection>(conf.getParameter<edm::InputTag>("recoEcalCands"));
  recoEcalCandL1SeededToken_ =  consumes<reco::RecoEcalCandidateCollection>(conf.getParameter<edm::InputTag>("recoEcalCandsL1Seeded"));
  ecalRecHitEBToken_ = consumes<EcalRecHitCollection>(conf.getParameter< edm::InputTag > ("ecalRecHitEB"));
  ecalRecHitEEToken_ = consumes<EcalRecHitCollection>(conf.getParameter< edm::InputTag > ("ecalRecHitEE"));
  genParticleToken_ = consumes<std::vector<reco::GenParticle> > (conf.getParameter< edm::InputTag >("genParticles"));
}

SpikeNtupleMaker::~SpikeNtupleMaker(){}

void SpikeNtupleMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  ntupTree_ = new TTree("spikeTree","Spike Tree");
  ntupTree_->Branch("evt",&evtData_,EvtInfoStruct::contents().c_str());
  ntupTree_->Branch("nrEG",&nrEG_,"nrEG/I");
  for(size_t i=0;i<egData_.size();i++){
    std::ostringstream branchLabel;
    branchLabel<<"eg"<<i+1;
    ntupTree_->Branch(branchLabel.str().c_str(),&egData_[i],SpikeData::contents().c_str());
  }
  ntupTree_->Branch("mp1",&mp1P4_,P4Struct::contents().c_str());
  ntupTree_->Branch("mp2",&mp2P4_,P4Struct::contents().c_str());
 
}


void SpikeNtupleMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<reco::RecoEcalCandidateCollection> recoEcalCandHandle;
  iEvent.getByToken(recoEcalCandToken_,recoEcalCandHandle);

  edm::Handle<reco::RecoEcalCandidateCollection> recoEcalCandL1SeededHandle;
  iEvent.getByToken(recoEcalCandL1SeededToken_,recoEcalCandL1SeededHandle);

  edm::Handle<EcalRecHitCollection> ecalRecHitEBHandle;
  iEvent.getByToken(ecalRecHitEBToken_,ecalRecHitEBHandle);
  
  edm::Handle<EcalRecHitCollection> ecalRecHitEEHandle;
  iEvent.getByToken(ecalRecHitEEToken_,ecalRecHitEEHandle);
  
  edm::Handle<std::vector<reco::GenParticle> > genParticleHandle;
  iEvent.getByToken(genParticleToken_,genParticleHandle);
  
 //  for(size_t partNr=0;partNr<genParticleHandle->size();partNr++){
//     const reco::GenParticle& genPart = (*genParticleHandle)[partNr];
//     std::cout <<"part "<<partNr<<" pdgId "<<genPart.pdgId()<<" status "<<genPart.status()<<" et "<<genPart.et()<<" energy "<<genPart.energy()<<std::endl;
//   }
  
  evtData_.fill(iEvent);
  const reco::GenParticle* mp1 = getPart(genParticleHandle,4110000);
  const reco::GenParticle* mp2 = getPart(genParticleHandle,-4110000);
  if(mp1) mp1P4_.fill(mp1->p4());
  else mp1P4_=-999.;
  if(mp2) mp2P4_.fill(mp2->p4());
  else mp2P4_=-999.;
		    
  nrEG_=0;
  std::for_each(egData_.begin(),egData_.end(),std::mem_fun_ref(&SpikeData::clear));
  //
  // std::cout <<"egData "<<egData_.size()<<" recoEcal "<<recoEcalCandHandle->size()<<std::endl;
  std::vector<const reco::RecoEcalCandidate*> tmpCands;
  for(size_t candNr=0;candNr<recoEcalCandHandle->size();candNr++){
    const reco::RecoEcalCandidate* cand = &(*recoEcalCandHandle)[candNr];
    tmpCands.push_back(cand);
  }
  std::sort(tmpCands.begin(),tmpCands.end(),[](const reco::RecoEcalCandidate* lhs,const reco::RecoEcalCandidate* rhs)
	    -> bool { return lhs->et() > rhs ->et();});
  
  for(size_t candNr=0;candNr<tmpCands.size() && candNr<egData_.size();candNr++){   
    bool isEB = tmpCands[candNr]->superCluster()->seed()->seed().subdetId()==EcalBarrel;
    const reco::RecoEcalCandidate* l1SeededCand = matchClusInCollection(tmpCands[candNr]->p4(),recoEcalCandL1SeededHandle,0.05,0.1);
    egData_[candNr].fill(*tmpCands[candNr],isEB ? *ecalRecHitEBHandle.product() : *ecalRecHitEEHandle.product(),l1SeededCand!=nullptr);
    nrEG_++;
  }
  



  ntupTree_->Fill();
  
}

const reco::RecoEcalCandidate* SpikeNtupleMaker::matchClusInCollection(const math::XYZTLorentzVector& p4,edm::Handle<std::vector<reco::RecoEcalCandidate> >& collection,const float bestDeltaR,float bestRelEtDiff)
{
  float bestDeltaR2=bestDeltaR*bestDeltaR;
  float eta= p4.Eta();
  float phi= p4.Phi();
  const reco::RecoEcalCandidate* bestCand=nullptr;
  if(!collection.isValid() || p4.Et()==0) return bestCand;
  for(size_t candNr=0;candNr<collection->size();candNr++){
    const reco::RecoEcalCandidate* cand = &(*collection)[candNr];
    float dR2= reco::deltaR2(eta,phi,cand->eta(),cand->phi());
    float relEtDiff = fabs(cand->et()-p4.Et())/p4.Et();
    if(dR2<bestDeltaR2 && (relEtDiff<bestRelEtDiff || bestRelEtDiff<0) ){
      bestDeltaR2=dR2;
      if(bestRelEtDiff>=0) bestRelEtDiff= relEtDiff;
      bestCand = cand;
    }
  }
  return bestCand;
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

const reco::GenParticle* SpikeNtupleMaker::getPart(edm::Handle<std::vector<reco::GenParticle> > genParts,int partId)
{
  
  for(size_t partNr=0;partNr<genParts->size();partNr++){
    const reco::GenParticle& genPart = (*genParts)[partNr];
    if(genPart.pdgId()==partId && genPart.status()==1) return &genPart;
  }
  return nullptr;
}

void SpikeData::fill(const reco::RecoEcalCandidate& cand,const EcalRecHitCollection& recHits,bool l1Matched)
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
  passL1 = l1Matched;

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
