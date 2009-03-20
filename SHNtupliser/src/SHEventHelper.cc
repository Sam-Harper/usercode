#include "SHarper/SHNtupliser/interface/SHEventHelper.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"

#include "SHarper/SHNtupliser/interface/HackedFuncs.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "TVector3.h"

SHEventHelper::SHEventHelper(int datasetCode,float eventWeight):
  datasetCode_(datasetCode),
  eventWeight_(eventWeight),
  isMC_(true),
  nrGenPartToStore_(30)
{
  initEcalHitVec_();
  initHcalHitVec_();
}

void SHEventHelper::makeSHEvent(const heep::Event & heepEvent, SHEvent& shEvent)const
{
  shEvent.clear(); //reseting the event 
  //it is currently critical that calo hits are filled first as they are used in constructing the basic clusters
  addCaloHits(heepEvent,shEvent);
  addEventPara(heepEvent,shEvent);
  addElectrons(heepEvent,shEvent);
  addSuperClusters(heepEvent,shEvent);

  addTrigInfo(heepEvent,shEvent);
  addJets(heepEvent,shEvent);
  addMet(heepEvent,shEvent);
  addMCParticles(heepEvent,shEvent);
  addIsolTrks(heepEvent,shEvent);
}

void SHEventHelper::addEventPara(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  shEvent.setRunnr(heepEvent.runnr());
  shEvent.setEventnr(heepEvent.eventnr());
  shEvent.setIsMC(isMC_);
  shEvent.setDatasetCode(datasetCode_);
  shEvent.setWeight(eventWeight_);
}


void SHEventHelper::addElectrons(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  const std::vector<heep::Ele>& electrons = heepEvent.heepElectrons();
  for(size_t eleNr=0;eleNr<electrons.size();eleNr++){
    shEvent.addElectron(electrons[eleNr],shEvent.getCaloHits());
  }
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
    shEvent.addTrigInfo(trigInfo);
  } 

}

void SHEventHelper::addJets(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  for(size_t jetNr=0;jetNr<heepEvent.jets().size();jetNr++){
    shEvent.addJet(heepEvent.jets()[jetNr]);
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
    shEvent.addMCParticle(index,status,pid,jmo1,jmo2,nrMo,jda1,jda2,nrDa,p4);
    
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
      return eeDetId.hashedIndex()+EBDetId::SIZE_HASH;
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
  std::vector<SHCaloHit> tempVector(EBDetId::SIZE_HASH+EEDetId::SIZE_HASH,nullHit);
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
	  ecalHitVec_[detId.hashedIndex()+EBDetId::SIZE_HASH].setDetId(detId.rawId());
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
