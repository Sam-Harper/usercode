#include "SHarper/SHNtupliser/interface/SHEvent.hh"

#include "SHarper/SHNtupliser/interface/SHBugFixFuncs.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

#include <iomanip>
//note there also exists a SHEvent_CMSSW.cc and SHEvent_standard.cc which define the CMSSW specific functions


ClassImp(SHEvent)

SHEvent::SHEvent():
  superClusArray_("SHSuperCluster",4),
  electronArray_("SHElectron",12),
  mcPartArray_("SHMCParticle",30),
  jetArray_("SHJet",10),
  caloHits_(),
  isolSuperClusArray_("SHIsolSuperCluster",5),
  isolClusArray_("SHIsolCluster",20),
  isolTrkArray_("SHIsolTrack",20),
  trigArray_("SHTrigInfo",20),
  runnr_(0),eventnr_(0),
  isMC_(0),datasetCode_(0),
  weight_(0.),
  metData_()
{
 
}

SHEvent::~SHEvent()
{
  superClusArray_.Delete();
  electronArray_.Delete();
  mcPartArray_.Delete();
  jetArray_.Delete();
  isolSuperClusArray_.Delete();
  isolClusArray_.Delete();
  isolTrkArray_.Delete();
  trigArray_.Delete();
  
}

void SHEvent::addMCParticle(int partIndx,int partStdhep,int partIdhep,
			    int partJmo1,int partJmo2,int partNrMo,
			    int partJda1,int partJda2,int partNrDa,
			    TLorentzVector& p4)
{
  if(partIndx!=nrMCParticles()) std::cout <<"SHEvent::addMCParticle : Error particle should be at index "<<partIndx<<" but will be added in index "<<nrMCParticles()<<std::endl;
  new(mcPartArray_[nrMCParticles()]) SHMCParticle(partIndx,partStdhep,partIdhep,
						  partJmo1,partJmo2,partNrMo,
						  partJda1,partJda2,partNrDa,p4);
}

void SHEvent::addMCParticle(const SHMCParticle& mcPart)
{
  if(mcPart.index()!=nrMCParticles()) std::cout <<"SHEvent::addMCParticle : Error particle should be at index "<<mcPart.index()<<" but will be added in index "<<nrMCParticles()<<std::endl;
  new(mcPartArray_[nrMCParticles()]) SHMCParticle(mcPart);
}

void SHEvent::addJet(const SHJet& jet)
{
  new(jetArray_[nrJets()]) SHJet(jet);
}

void SHEvent::addElectron(const SHElectron& ele,const SHSuperCluster& superClus)
{
  int superClusIndx = getSuperClusIndx(superClus.rawNrgy(),superClus.eta(),superClus.phi());
  if(superClusIndx==-1){ //super clus has not been added already so add it
    superClusIndx = nrSuperClus();
    new(superClusArray_[superClusIndx]) SHSuperCluster(superClus);
  }  
  new(electronArray_[nrElectrons()]) SHElectron(ele);
  getElectron(nrElectrons()-1)->setSuperClusIndex(superClusIndx); //-1 as it counts from zero
  getElectron(nrElectrons()-1)->setMotherEvent(this);
}

void SHEvent::addElectron(const SHElectron& ele)
{
  int superClusIndx=-1;//as no supercluster is assoicated
  new(electronArray_[nrElectrons()]) SHElectron(ele);
  getElectron(nrElectrons()-1)->setSuperClusIndex(superClusIndx);
  getElectron(nrElectrons()-1)->setMotherEvent(this);
}

void SHEvent::addIsolInfo(const SHEvent& rhs)
{
  TempFuncs::copyTClonesArray<SHIsolSuperCluster>(isolSuperClusArray_,rhs.isolSuperClusArray_);
  TempFuncs::copyTClonesArray<SHIsolCluster>(isolClusArray_,rhs.isolClusArray_);
  TempFuncs::copyTClonesArray<SHIsolTrack>(isolTrkArray_,rhs.isolTrkArray_);
}

void SHEvent::addCaloHits(const SHEvent& rhs)
{
  caloHits_ = rhs.caloHits_;
}

void SHEvent::addIsolTrk(const SHIsolTrack& trk)
{
  new(isolTrkArray_[nrIsolTrks()]) SHIsolTrack(trk);
  
}

void SHEvent::addIsolTrk(const TVector3& p3,const TVector3& vtxPos,bool posCharge)
{
  new(isolTrkArray_[nrIsolTrks()]) SHIsolTrack(p3,vtxPos,posCharge);
  
}

void SHEvent::addTrigInfo(const SHTrigInfo& trigInfo)
{
  new(trigArray_[nrTrigs()]) SHTrigInfo(trigInfo);
  
}

//we only store hits which actually have information
void SHEvent::addEcalHits(const std::vector<SHCaloHit> & hitVec)
{
  for(size_t hitNr=0;hitNr<hitVec.size();hitNr++){
    if(hitVec[hitNr].nrgy()>=-100){
      caloHits_.addHit(hitVec[hitNr]);
    }
  }
}

//we only store hits which actually have information
void SHEvent::addHcalHits(const std::vector<SHCaloHit> & hitVec)
{
  for(size_t hitNr=0;hitNr<hitVec.size();hitNr++){
    if(hitVec[hitNr].nrgy()>=-100){
      caloHits_.addHit(hitVec[hitNr]);
    }
  }
} 

const SHSuperCluster* SHEvent::getSuperClus(int clusNr)const
{
  SHSuperCluster* clus = (SHSuperCluster*) superClusArray_[clusNr];
  return clus;
}

SHSuperCluster* SHEvent::getSuperClus_(int clusNr)
{
  SHSuperCluster* clus = (SHSuperCluster*) superClusArray_[clusNr];
  return clus;
}

const SHMCParticle* SHEvent::getMCParticle(int partNr)const
{
  SHMCParticle* part = (SHMCParticle*) mcPartArray_[partNr];
  return part;
}

const SHElectron* SHEvent::getElectron(int eleNr)const
{
  SHElectron* ele = (SHElectron*) electronArray_[eleNr];
  ele->setMotherEvent(this);
  return ele;
}

const SHJet* SHEvent::getJet(int jetNr)const
{
  SHJet* jet = (SHJet*) jetArray_[jetNr];
  return jet;
}

SHJet* SHEvent::getJet(int jetNr)
{
  SHJet* jet = (SHJet*) jetArray_[jetNr];
  return jet;
}

const SHIsolTrack* SHEvent::getIsolTrk(int trkNr)const
{
  SHIsolTrack* trk = (SHIsolTrack*) isolTrkArray_[trkNr];
  return trk;
}

const SHTrigInfo* SHEvent::getTrigInfo(int trigNr)const
{
  SHTrigInfo* trigInfo = (SHTrigInfo*) trigArray_[trigNr];
  return trigInfo;
}



//non const acccess
SHElectron* SHEvent::getElectron(int eleNr)
{
  SHElectron* ele = (SHElectron*) electronArray_[eleNr];
  ele->setMotherEvent(this);
  return ele;
}

const SHIsolCluster* SHEvent::getIsolClus(int clusNr)const
{
  SHIsolCluster* clus = (SHIsolCluster*) isolClusArray_[clusNr];
  return clus;
}

const SHIsolSuperCluster* SHEvent::getIsolSuperClus(int clusNr)const
{
  SHIsolSuperCluster* clus = (SHIsolSuperCluster*) isolSuperClusArray_[clusNr];
  return clus;
}

int SHEvent::getSuperClusIndx(float rawNrgy,float eta,float phi)const
{
  for(int superClusNr=0;superClusNr<nrSuperClus();superClusNr++){
    const SHSuperCluster* superClus = getSuperClus(superClusNr);
    float dNrgyAbs = fabs(superClus->rawNrgy()-rawNrgy);
    float dEtaAbs = fabs(superClus->eta()-eta);
    float dPhiAbs = fabs(superClus->phi()-phi);
    if(dNrgyAbs<1.0E-10 && dEtaAbs<1.0E-10 && dPhiAbs<1.0E-10) return superClusNr;
  }
  return -1;//didnt find it
}

int SHEvent::getIsolClusIndx(float rawNrgy,float eta,float phi)const
{
  for(int clusNr=0;clusNr<nrIsolClus();clusNr++){
    const SHIsolCluster* clus = getIsolClus(clusNr);
    float dNrgyAbs = fabs(clus->nrgy()-rawNrgy);
    float dEtaAbs = fabs(clus->eta()-eta);
    float dPhiAbs = fabs(clus->phi()-phi);
    if(dNrgyAbs<1.0E-10 && dEtaAbs<1.0E-10 && dPhiAbs<1.0E-10) return clusNr;
  }
  return -1;//didnt find it
}

void SHEvent::copyEventPara(const SHEvent& rhs)
{
  runnr_=rhs.runnr_;
  eventnr_=rhs.eventnr_;
  isMC_=rhs.isMC_;
  datasetCode_=rhs.datasetCode_;
  weight_=rhs.weight_;
  metData_=rhs.metData_;
}

//I have a memory leak from some where....
void SHEvent::clear()
{
  superClusArray_.Delete();
  electronArray_.Delete();
  mcPartArray_.Delete();
  jetArray_.Delete();
  isolSuperClusArray_.Delete();
  isolClusArray_.Delete();
  isolTrkArray_.Delete();
  trigArray_.Delete();
  caloHits_.clear();
  runnr_=0;
  eventnr_=0;
  isMC_=0;
  datasetCode_=0;
  weight_=0.;
  metData_.clear();
}




void SHEvent::flushTempData()const
{
  caloHits_.flushIndxTable();
}


void SHEvent::printTruth(int nrLines)const
{
  if(nrMCParticles()>0){
    std::cout << std::setw(2)<<" "<< std::setw(6) << "indx" << std::setw(8) << "stdhep" << std::setw(6) << "idhep" 
	      << std::setw(6) << "jmo1" << std::setw(6) << "jmo2" << std::setw(6) << "jda1" << std::setw(6) << "jda2" 
	      << std::setw(6) << "Px" << std::setw(10) << "Py" << std::setw(10) << "Pz" << std::setw(10) <<"E" 
	      << std::setw(10) << "Pt" <<std::setw(15)<< "Mass" << "\n";
    for(int i=0;i<nrMCParticles() && (i<nrLines || nrLines==-1);i++){
      const SHMCParticle *mcPart = getMCParticle(i);
      std::cout << *mcPart <<std::endl;
    }
  }
}

//this has to do some bug fixing as very high and high et trigger objs may not be stored
int SHEvent::getTrigCode()const
{
  int trigCode = 0x0;

  for(int trigNr=0;trigNr<3;trigNr++){ //very high and high broken
    if(!getTrigInfo(trigNr)->passTrig()) trigCode |=(0x1<<trigNr);
  }
  
  return trigCode;
}


int SHEvent::getTrigCode(double eta,double phi)
{
  int trigCode = 0x0;
  
  for(int trigNr=0;trigNr<3;trigNr++){ //very high and high broken
    if(!getTrigInfo(trigNr)->passTrig(eta,phi)) trigCode |=(0x1<<trigNr);
  }

  

  return trigCode;
}


void SHEvent::removeDupEles(std::vector<int>& dupEleNrs)
{
  std::vector<SHElectron*> goodEle;
  std::vector<SHElectron*> dupEle;
  for(int eleNr=0;eleNr<nrElectrons();eleNr++){
    if(!std::binary_search(dupEleNrs.begin(),dupEleNrs.end(),eleNr)) goodEle.push_back(getElectron(eleNr));
    else dupEle.push_back(getElectron(eleNr));
  }

  electronArray_.Clear();
  
  for(size_t eleNr=0;eleNr<goodEle.size();eleNr++){
    addElectron(*goodEle[eleNr],*goodEle[eleNr]->superClus());
  }
 
  //I think root has plans for the memory allocated and still owns it

  //for(size_t dupEleNr=0;dupEleNr<dupEle.size();dupEleNr++){
  //   delete dupEle[dupEleNr];
  // }


}
