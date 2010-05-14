#include "SHarper/SHNtupliser/interface/SHEventHelper.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"

#include "SHarper/SHNtupliser/interface/HackedFuncs.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"
#include "SHarper/SHNtupliser/interface/SHEleCMSSWStructs.hh"

#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "TrackingTools/GsfTools/interface/MultiTrajectoryStateTransform.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include "TVector3.h"
#include "TBits.h"

SHEventHelper::SHEventHelper(int datasetCode,float eventWeight):
  datasetCode_(datasetCode),
  eventWeight_(eventWeight),
  isMC_(true),
  nrGenPartToStore_(30)
{
  initEcalHitVec_();
  initHcalHitVec_();
}
void SHEventHelper::setup(const edm::ParameterSet& conf)
{ 
  minEtToPromoteSC_ = conf.getParameter<double>("minEtToPromoteSC");
  addMet_ = conf.getParameter<bool>("addMet");
  addJets_ = conf.getParameter<bool>("addJets");
  fillFromGsfEle_ = conf.getParameter<bool>("fillFromGsfEle");
  tracklessEleMaker_.setup(conf);
}

void SHEventHelper::makeSHEvent(const heep::Event & heepEvent, SHEvent& shEvent)const
{   
  std::cout <<"making event "<<std::endl;
  
  shEvent.clear(); //reseting the event 
  //it is currently critical that calo hits are filled first as they are used in constructing the basic clusters
  addCaloHits(heepEvent,shEvent);
  addEventPara(heepEvent,shEvent); //this must be filled second (ele + mu need beam spot info)
  addSuperClusters(heepEvent,shEvent);
  addElectrons(heepEvent,shEvent);
  addMuons(heepEvent,shEvent);

  std::cout <<"added muons "<<std::endl;

   addTrigInfo(heepEvent,shEvent);
  // addL1Info(heepEvent,shEvent); //due to a bug l1 info is not stored in summer 09 samples
   if(addJets_) addJets(heepEvent,shEvent);
   if(addMet_) addMet(heepEvent,shEvent);
   addMCParticles(heepEvent,shEvent);
   addIsolTrks(heepEvent,shEvent);
   std::cout <<"made event "<<std::endl;
}

void SHEventHelper::addEventPara(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  // std::cout <<"atart"<<std::endl;
  //std::cout <<" adding event "<<&(heepEvent.event())<<std::endl;
  //std::cout <<"run nr "<< heepEvent.runnr()<<std::endl;
  //std::cout <<"getting run nr "<<std::endl;
  shEvent.setRunnr(heepEvent.runnr());
  //std::cout <<"eventnr "<<std::endl;
  shEvent.setEventnr(heepEvent.eventnr());
  //std::cout <<"mv "<<std::endl;
  shEvent.setIsMC(isMC_);
  //std::cout <<"dataset "<<std::endl;
  shEvent.setDatasetCode(datasetCode_);
  //std::cout <<"weight "<<std::endl;
  shEvent.setWeight(eventWeight_);
  //std::cout <<"gen pt hat "<<std::endl;
  shEvent.setGenEventPtHat(heepEvent.genEventPtHat());
  shEvent.setBX(heepEvent.bx());
  shEvent.setLumiSec(heepEvent.lumiSec());
  shEvent.setTime(heepEvent.time());
  shEvent.setOrbitNumber(heepEvent.orbitNumber());
  shEvent.setNrVertices(heepEvent.handles().vertices->size());
  TVector3 vtxPos;
  if(heepEvent.handles().vertices->size()>0){  
    const reco::Vertex& vertex = heepEvent.handles().vertices->front();
    vtxPos.SetXYZ(vertex.x(),vertex.y(),vertex.z()); 
  }
  shEvent.setVertex(vtxPos);
  math::XYZPoint bsCMSSW = heepEvent.handles().beamSpot->position();
  TVector3 bs(bsCMSSW.x(),bsCMSSW.y(),bsCMSSW.z());
  shEvent.setBeamSpot(bs);
  
 
  //std::cout <<"done "<<std::endl;
}

//this now promotes all sc to electrons...
//runs through all sc + works out if matched to electron
//if so, just adds it as normal
//if not, makes a superclus only electron
void SHEventHelper::addElectrons(const heep::Event& heepEvent, SHEvent& shEvent)const
{  
 
  // const std::vector<heep::Ele>& electrons = heepEvent.heepEles();
  const std::vector<reco::GsfElectron>& electrons = heepEvent.gsfEles();
  const std::vector<reco::SuperCluster>& superClusEB = heepEvent.superClustersEB(); 
  const std::vector<reco::SuperCluster>& superClusEE = heepEvent.superClustersEE();
  
  for(size_t scNr=0;scNr<superClusEB.size();scNr++){
    std::cout <<"eb seed id "<<superClusEB[scNr].seed()->seed().rawId()<<scNr<<"/"<<superClusEE.size()<<std::endl;

    size_t eleNr = matchToEle(superClusEB[scNr],electrons);
    if(eleNr<electrons.size()) addElectron(heepEvent,shEvent,electrons[eleNr]);
    else if(superClusEB[scNr].energy()*sin(superClusEB[scNr].position().theta())>minEtToPromoteSC_ && minEtToPromoteSC_<10000) addElectron(heepEvent,shEvent,superClusEB[scNr]);
  }
  for(size_t scNr=0;scNr<superClusEE.size();scNr++){
    std::cout <<"ee seed id "<<superClusEB[scNr].seed()->seed().rawId()<<" "<<scNr<<"/"<<superClusEE.size()<<" energy "<<superClusEE[scNr].energy()<<std::endl;
    size_t eleNr = matchToEle(superClusEE[scNr],electrons);
    std::cout <<"ele Nr "<<eleNr<<" eles size "<<electrons.size()<<std::endl;
    std::cout <<"ele et "<<superClusEE[scNr].energy()*sin(superClusEE[scNr].position().theta())<<std::endl;
    if(eleNr<electrons.size()) addElectron(heepEvent,shEvent,electrons[eleNr]);
    else if(superClusEE[scNr].energy()*sin(superClusEE[scNr].position().theta())>minEtToPromoteSC_ && minEtToPromoteSC_<10000) addElectron(heepEvent,shEvent,superClusEE[scNr]);
  }
  
  
}

void SHEventHelper::addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const reco::GsfElectron& gsfEle)const
{
  MultiTrajectoryStateTransform trajStateTransform(heepEvent.handles().trackGeom.product(),heepEvent.handles().bField.product());
  shEvent.addElectron(gsfEle,shEvent.getCaloHits());
  SHElectron* shEle = shEvent.getElectron(shEvent.nrElectrons()-1);
  
  if(shEle->seedId()!=0){
    
    const TVector3& seedPos = GeomFuncs::getCell(shEle->seedId()).pos();
    GlobalPoint posGP(seedPos.X(),seedPos.Y(),seedPos.Z());  
    
    //  std::cout <<" pt "<<gsfEle.gsfTrack()->pt()<<std::endl;
    TrajectoryStateOnSurface outTSOS = trajStateTransform.outerStateOnSurface(*gsfEle.gsfTrack());
    TrajectoryStateOnSurface innTSOS = trajStateTransform.innerStateOnSurface(*gsfEle.gsfTrack());
    TrajectoryStateOnSurface outToSeedTSOS = trajStateTransform.extrapolatedState(outTSOS,posGP);
    TrajectoryStateOnSurface innToSeedTSOS = trajStateTransform.extrapolatedState(innTSOS,posGP);
    
    TVector3 dummy; 
    dummy.SetPtEtaPhi(0.00001,-10,0);
    if(outToSeedTSOS.isValid()){
      TVector3 outToSeedPos(outToSeedTSOS.globalPosition().x(),outToSeedTSOS.globalPosition().y(),outToSeedTSOS.globalPosition().z()); 
      shEle->setPosTrackOutToSeed(outToSeedPos); 
    }else shEle->setPosTrackOutToSeed(dummy);
    
    if(innToSeedTSOS.isValid()){
      TVector3 innToSeedPos(innToSeedTSOS.globalPosition().x(),innToSeedTSOS.globalPosition().y(),innToSeedTSOS.globalPosition().z());
      shEle->setPosTrackInnToSeed(innToSeedPos);  
    }else shEle->setPosTrackInnToSeed(dummy);
    //  std::cout <<"set state"<<std::endl;
  }
  
}

void SHEventHelper::addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const reco::SuperCluster& superClus)const
{
  std::cout <<"adding electron "<<std::endl;
  cmssw::IsolationVariables isol03 = tracklessEleMaker_.getIsol(superClus,heepEvent,0.3);
 std::cout <<"done 03 "<<std::endl;
  cmssw::IsolationVariables isol04 = tracklessEleMaker_.getIsol(superClus,heepEvent,0.4);
  std::cout <<"done 04 "<<std::endl;
  cmssw::FiducialFlags fid = tracklessEleMaker_.getFid(superClus,heepEvent);
   std::cout <<"done fid "<<std::endl;
  cmssw::ShowerShape shape = tracklessEleMaker_.getShowerShape(superClus,heepEvent,fid.isEB);
  std::cout <<"done showershape "<<std::endl;
  TLorentzVector p4 = tracklessEleMaker_.getP4(superClus,heepEvent);
  std::cout <<"done p4 "<<std::endl;
  shEvent.addElectron(p4,superClus,fid,shape,isol03,isol04,shEvent.getCaloHits());
  std::cout <<"done"<<std::endl;
}

void SHEventHelper::addMuons(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  for(size_t muNr=0;muNr<heepEvent.muons().size();muNr++){
    const reco::Muon& muon = heepEvent.muons()[muNr];

    if(muon.isGlobalMuon()) shEvent.addMuon(muon);
  }

}

size_t SHEventHelper::matchToEle(const reco::SuperCluster& superClus,const std::vector<reco::GsfElectron> eles)const
{
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    const reco::GsfElectron& ele = eles[eleNr];
    if(ele.superCluster()->seed()->hitsAndFractions()[0].first==superClus.seed()->hitsAndFractions()[0].first) return eleNr;
  }
  return eles.size();
}

size_t SHEventHelper::matchToEle(const reco::SuperCluster& superClus,const std::vector<heep::Ele> eles)const
{
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    const reco::GsfElectron& ele = eles[eleNr].gsfEle();
    if(ele.superCluster()->seed()->hitsAndFractions()[0].first==superClus.seed()->hitsAndFractions()[0].first) return eleNr;
  }
  return eles.size();
}

void SHEventHelper::addSuperClusters(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  const std::vector<reco::SuperCluster>& superClusEB = heepEvent.superClustersEB(); 
  for(size_t superClusNr=0;superClusNr<superClusEB.size();superClusNr++){
    shEvent.addSuperCluster(superClusEB[superClusNr],shEvent.getCaloHits());
  }
  //now endcap
  const std::vector<reco::SuperCluster>& superClusEE = heepEvent.superClustersEE(); 
  for(size_t superClusNr=0;superClusNr<superClusEE.size();superClusNr++){
    shEvent.addSuperCluster(superClusEE[superClusNr],shEvent.getCaloHits());
  }
}

void SHEventHelper::addCaloHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  addEcalHits(heepEvent,shEvent);
  addHcalHits(heepEvent,shEvent);
}

//we first zero the vector of the ecal hits (note we keep this persistant because we dont want to keep allocating memory)
//we then loop over the barrel and endcap hits and add them to the vector
//we then add the vector to the event
void SHEventHelper::addEcalHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  for(size_t i=0;i<ecalHitVec_.size();i++) ecalHitVec_[i].setNrgy(-999.);
  
  const EcalRecHitCollection* ebHits = heepEvent.ebHitsFull();
  const EcalRecHitCollection* eeHits = heepEvent.eeHitsFull();
  
  if(ebHits!=NULL){
    for(EcalRecHitCollection::const_iterator hitIt = ebHits->begin();
	hitIt!=ebHits->end();++hitIt){
      ecalHitVec_[ecalHitHash_(hitIt->detid())].setNrgy(hitIt->energy());
    }
  }//end of null check on ebHits

  if(eeHits!=NULL){
    for(EcalRecHitCollection::const_iterator hitIt = eeHits->begin();
	hitIt!=eeHits->end();++hitIt){
      ecalHitVec_[ecalHitHash_(hitIt->detid())].setNrgy(hitIt->energy());
    }
  }//end of null check on eeHits

  shEvent.addEcalHits(ecalHitVec_);

}

void SHEventHelper::addHcalHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  for(size_t i=0;i<hcalHitVec_.size();i++) hcalHitVec_[i].setNrgy(-999.);
  
  const HBHERecHitCollection* hcalHits = heepEvent.hbheHits();
  
  if(hcalHits!=NULL){
    for(HBHERecHitCollection::const_iterator hitIt = hcalHits->begin();
	hitIt!=hcalHits->end();++hitIt){
      hcalHitVec_[hcalHitHash_(hitIt->detid())].setNrgy(hitIt->energy());
    }
  }//end of null check on hcal hits

  shEvent.addHcalHits(hcalHitVec_);
}

void SHEventHelper::addTrigInfo(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  const trigger::TriggerEvent& trigEvt = heepEvent.triggerEvent();
  //  const trigger::TriggerObjectCollection& trigObjs = heepEvent.trigObjColl();
  for(size_t filterNr=0;filterNr<trigEvt.sizeFilters();filterNr++){
    SHTrigInfo trigInfo;
    trigInfo.setTrigId(-1);
    trigInfo.setTrigName(trigEvt.filterTag(filterNr).label());
    const trigger::Keys& trigKeys = trigEvt.filterKeys(filterNr);  //trigger::Keys is actually a vector<uint16_t> holding the position of trigger objects in the trigger collection passing the filter
    const trigger::TriggerObjectCollection & trigObjColl(trigEvt.getObjects());
    for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){
      const trigger::TriggerObject& obj = trigObjColl[*keyIt];
      TLorentzVector p4;
      //note I call this function as its probably the fastest way to get info out of  TriggerObject in 22X (look at how it calculates et, its impressive, it might be possible to do it slower but I doubt it)
      p4.SetPtEtaPhiM(obj.pt(),obj.eta(),obj.phi(),obj.mass());
      trigInfo.addObj(p4);
    }
    if(!trigKeys.empty()) shEvent.addTrigInfo(trigInfo); //only adding triggers which actually have objects passing
  } 

}

void SHEventHelper::addL1Info(const heep::Event& heepEvent,SHEvent& shEvent)const
{
 
  const std::vector<bool>& l1Word = heepEvent.l1Decision(); 
  TBits myL1Bits(l1Word.size()); 
  for(size_t bitNr=0;bitNr<l1Word.size();bitNr++){
    if(l1Word[bitNr]) myL1Bits.SetBitNumber(bitNr);
  }
  shEvent.setL1Bits(myL1Bits);
  for(size_t candNr=0;candNr<heepEvent.l1EmNonIso().size();candNr++){
    const l1extra::L1EmParticle& cand = heepEvent.l1EmNonIso()[candNr];
    TLorentzVector p4;
    p4.SetPxPyPzE(cand.px(),cand.py(),cand.pz(),cand.energy());
    shEvent.addL1Cand(p4,cand.type());
  }
  for(size_t candNr=0;candNr<heepEvent.l1EmIso().size();candNr++){
    const l1extra::L1EmParticle& cand = heepEvent.l1EmIso()[candNr];
    TLorentzVector p4;
    p4.SetPxPyPzE(cand.px(),cand.py(),cand.pz(),cand.energy());
    shEvent.addL1Cand(p4,cand.type());
  }
}
  

void SHEventHelper::addJets(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  for(size_t jetNr=0;jetNr<heepEvent.jets().size();jetNr++){
    if(heepEvent.jets()[jetNr].et()>15) shEvent.addJet(heepEvent.jets()[jetNr]);
  }
}

void SHEventHelper::addIsolTrks(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  TVector3 trkP3,trkVtxPos;
  const float minPtCut=1.0;
  const std::vector<reco::Track>& tracks = heepEvent.ctfTracks();
 
  for(size_t trkNr=0;trkNr<tracks.size();trkNr++){
    const reco::Track& trk = tracks[trkNr];
    trkP3.SetXYZ(trk.px(),trk.py(),trk.pz());
    trkVtxPos.SetXYZ(trk.vx(),trk.vy(),trk.vz());
    if(trkP3.Pt()>minPtCut) shEvent.addIsolTrk(trkP3,trkVtxPos,trk.charge()>0);
  }
} 

void SHEventHelper::addMet(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  //why yes I might be throwing away the patty goodness, whoops
  const pat::MET& met = heepEvent.mets()[0];
  SHMet shMet;
  shMet.setMet(met.mEtCorr()[0].mex,met.mEtCorr()[0].mey);
  shMet.setSumEmEt(met.emEtInEB(),met.emEtInEE(),met.emEtInHF());
  shMet.setSumHadEt(met.hadEtInHB(),met.hadEtInHE(),met.hadEtInHO(),met.hadEtInHF());
  shEvent.setMet(shMet);
}  

void SHEventHelper::addMCParticles(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(!heepEvent.hasGenParticles()) return;
  const std::vector<reco::GenParticle>& particles = heepEvent.genParticles();
  int nrPartsToStore = nrGenPartToStore_;
  if(nrPartsToStore==-1) nrPartsToStore = particles.size();

  //okay its really just easier if I make a vector of pointers and call it a day
  //this probably could be more efficient 
  std::vector<const reco::Candidate*> candPointers(particles.size(),0x0);
  for(size_t partNr=0;partNr<particles.size();partNr++){
    candPointers[partNr]=&particles[partNr];
  }
  
  for(size_t partIndx=0;partIndx<candPointers.size() && partIndx<static_cast<unsigned>(nrPartsToStore);partIndx++){
    
    //first get all the easy quantities
    const reco::Candidate* genPart = candPointers[partIndx];
    int index = partIndx;
    int pid = genPart->pdgId();
    int status = genPart->status(); //note this is not accessible in CMSSW_1_3_5
    //int status = -1;
    int nrMo = genPart->numberOfMothers();
    int nrDa = genPart->numberOfDaughters();
    TLorentzVector p4(genPart->px(),genPart->py(),genPart->pz(),genPart->energy());
    TVector3 pos(genPart->vx(),genPart->vy(),genPart->vz());
    
    
    //now sort out the mother and daughter links
    int jmo1 = -1;
    int jmo2 = -1;
    int jda1 = -1;
    int jda2 = -1;
    std::vector<const reco::Candidate*>::const_iterator vecIt;
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->mother(0));
    if(vecIt!=candPointers.end()) jmo1 = vecIt - candPointers.begin();
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->mother(nrMo-1));
    if(vecIt!=candPointers.end()) jmo2 = vecIt - candPointers.begin();
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->daughter(0));
    if(vecIt!=candPointers.end()) jda1 = vecIt - candPointers.begin();
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->daughter(nrDa-1));
    
    if(vecIt!=candPointers.end()) jda2 = vecIt - candPointers.begin();

    //yay, we have everything we need
    shEvent.addMCParticle(index,status,pid,jmo1,jmo2,nrMo,jda1,jda2,nrDa,p4,pos);
    
  }//end loop over all mc particles

}

int SHEventHelper::ecalHitHash_(const DetId detId)const
{
  if(detId.det()==DetId::Ecal){
    if(detId.subdetId()==EcalBarrel){
      EBDetId ebDetId(detId);
      return ebDetId.hashedIndex();
    }else if(detId.subdetId()==EcalEndcap){
      EEDetId eeDetId(detId);
      return eeDetId.hashedIndex()+EBDetId::MAX_HASH+1;
    }
  } 
  return -1; //not in ecal barrel or endcap  
}


int SHEventHelper::hcalHitHash_(const DetId detId)const
{
  HcalDetId hcalDetId(detId);
  return hcalDetId.hashed_index();
}


void SHEventHelper::initEcalHitVec_()const
{
  //first make sure the vector is the approprate size and has all null entries
  SHCaloHit nullHit(0,-999.);
  std::vector<SHCaloHit> tempVector(EBDetId::MAX_HASH+1+(EEDetId::kSizeForDenseIndexing),nullHit);
  ecalHitVec_.swap(tempVector);

  
  //now init barrel, place det ids at the correct positions
  for(int barrelIEta=-85;barrelIEta<=85;barrelIEta++){
    if(barrelIEta==0) continue;
    for(int barrelIPhi=1;barrelIPhi<=360;barrelIPhi++){
      EBDetId detId(barrelIEta,barrelIPhi);
      ecalHitVec_[detId.hashedIndex()].setDetId(detId.rawId());
    }
  }

  //now init endcap
  for(int xNr=1;xNr<=100;xNr++){
    for(int yNr=1;yNr<=100;yNr++){
      for(int zSide=-1;zSide<=1;zSide+=2){
	if(EEDetId::validDetId(xNr,yNr,zSide)){
	  EEDetId detId(xNr,yNr,zSide);
	  ecalHitVec_[detId.hashedIndex()+EBDetId::MAX_HASH+1].setDetId(detId.rawId());
	}//end valid det id check
      }//z side loop
    }//end phi loop
  }//end eta loop

  //std::cout <<"checking indexes"<<std::endl;
  //for(size_t i=0;i<ecalHitVec_.size();i++){
    //if(ecalHitVec_[i].detId()==0) std::cout <<"ecal index "<<i<< " is "<<ecalHitVec_[i].detId()<<std::endl;
    // }

}


void SHEventHelper::initHcalHitVec_()const
{
  SHCaloHit nullHit(0,-999.);
  std::vector<SHCaloHit> tempVector(kNrHcalBarrelCrys_+kNrHcalEndcapCrys_,nullHit);
  hcalHitVec_.swap(tempVector);

  //fill barrel and endcap seperately
  //first barrel
  for(int iEtaAbs=1;iEtaAbs<=16;iEtaAbs++){
    for(int side=0;side<=1;side++){
       int iEta = iEtaAbs*(2*side-1);
       for(int iPhi=1;iPhi<=72;iPhi++){
	 int maxDepth = 1;
	 if(iEtaAbs>=15) maxDepth=2;
	 for(int depth=1;depth<=maxDepth;depth++){
	   if(HackedFuncs::validHcalDetId(HcalBarrel,iEta,iPhi,depth)){
	     HcalDetId detId(HcalBarrel,iEta,iPhi,depth);
	     int indx = detId.hashed_index();
	   
	    //std::cout <<"iEta "<<iEta<<" iPhi "<<iPhi<<" depth "<<depth<< " pos phi "<<pos.Phi()<<" eta "<<pos.Eta()<<" index "<<indx;//<<std::endl; 
	     

	    hcalHitVec_[indx].setDetId(detId.rawId());
	   
	   }//end vaild det id
	 }//end depth loop 
       }//end iphi loop
    }//end side loop
  }//end barrel ieta loop

  for(int iEtaAbs=16;iEtaAbs<=29;iEtaAbs++){
    for(int side=0;side<=1;side++){
      int iEta = iEtaAbs*(2*side-1);
      for(int iPhi=1;iPhi<=72;iPhi++){
	for(int depth=1;depth<=3;depth++){
	  if(HackedFuncs::validHcalDetId(HcalEndcap,iEta,iPhi,depth)){
	    HcalDetId detId(HcalEndcap,iEta,iPhi,depth);
	    int indx = detId.hashed_index();
	   
	    // std::cout <<"iEta "<<iEta<<" iPhi "<<iPhi<<" depth "<<depth<< " pos phi "<<pos.Phi()<<" eta "<<pos.Eta()<<" index "<<indx;//<<std::endl; 
	  
	    hcalHitVec_[indx].setDetId(detId.rawId());
	    
	    // std::cout <<" detId "<<hcalHitVec_[indx].detId()<<std::endl;
	  }
	}//end depth loop
      }//end iphi loop
    }//end side loop
  }//end iEtaAbs loop
  std::cout <<"checking indexes"<<std::endl;
  for(size_t i=0;i<hcalHitVec_.size();i++){
    if(hcalHitVec_[i].detId()==0) std::cout <<"hcal index "<<i<< " is "<<hcalHitVec_[i].detId()<<std::endl;
  }
}
