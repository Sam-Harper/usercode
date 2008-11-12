#include "SHarper/SHNtupliser/interface/SHEvent.hh"

//evil nasty CMSSW stuff
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEle.h"


//note: change pointers to reference variables
void SHEvent::addElectron(const heep::Ele& ele,const SHCaloHitContainer& hits)
{
  const reco::SuperCluster& superClus = ele.superCluster();

  //now we need to figure out if the supercluster has already been added to event
  //this is due to an unexpected behaviour of CMSSW, if have multiple tracks pointing to each supercluster, we get multiple electrons assoicated with each SC
  int superClusIndx = getSuperClusIndx(superClus.rawEnergy(),superClus.position().Eta(),superClus.position().Phi()); //matches on raw energy, eta and phi in calo, returns -1 if not found

  if(superClusIndx==-1){//not already added, need to add to event
    superClusIndx=nrSuperClus();
    new(superClusArray_[superClusIndx]) SHSuperCluster(superClus,hits);
  }
  
  new(electronArray_[nrElectrons()]) SHElectron(ele,superClusIndx);
  getElectron(nrElectrons()-1)->setMotherEvent(this); //telling the electron which event owns it
 
}


void SHEvent::addJet(const pat::Jet& jet)
{
  new(jetArray_[nrJets()]) SHJet(jet);
}

void SHEvent::addIsolSuperCluster(const reco::SuperCluster& superClus)
{
  //first make the supercluster
  float nrgy = superClus.energy();
  float rawNrgy = superClus.rawEnergy();
  TVector3 pos(superClus.x(),superClus.y(),superClus.z());
  new(isolSuperClusArray_[nrIsolSuperClus()]) SHIsolSuperCluster(nrgy,rawNrgy,pos);
  SHIsolSuperCluster* shSuperClus = (SHIsolSuperCluster*) isolSuperClusArray_[nrIsolSuperClus()-1];

  //now add the basic clusters to the event if not already addeded
  for(reco::basicCluster_iterator clusIt=superClus.clustersBegin();
      clusIt!=superClus.clustersEnd();++clusIt){
    int clusIndx = getIsolClusIndx((*clusIt)->energy(),(*clusIt)->position().Eta(),(*clusIt)->position().Phi());
    if(clusIndx==-1){//not already added, need to add
      clusIndx=nrIsolClus();
      float clusNrgy((*clusIt)->energy());
      TVector3 clusPos((*clusIt)->x(),(*clusIt)->y(),(*clusIt)->z());
      const std::vector<DetId>& detIds = (*clusIt)->getHitsByDetId();
      std::vector<int> detIdsAsInts(detIds.size(),0);
      for(size_t i=0;i<detIds.size();i++) detIdsAsInts[i]=detIds[i].rawId();
     
      new(isolClusArray_[nrIsolClus()]) SHIsolCluster(clusNrgy,clusPos,detIdsAsInts);
    }
    shSuperClus->addBasicCluster(clusIndx);
  }//end basic cluster loop
}
   
void SHEvent::addIsolCluster(const reco::BasicCluster& clus)
{
  //first check if its been added before
  int clusIndx = getIsolClusIndx(clus.energy(),clus.position().Eta(),clus.position().Phi());
  if(clusIndx==-1){ //it hasnt
    float clusNrgy(clus.energy());
    TVector3 clusPos(clus.x(),clus.y(),clus.z());
    const std::vector<DetId>& detIds = clus.getHitsByDetId();
    std::vector<int> detIdsAsInts(detIds.size(),0);
    for(size_t i=0;i<detIds.size();i++) detIdsAsInts[i]=detIds[i].rawId();
    new(isolClusArray_[nrIsolClus()]) SHIsolCluster(clusNrgy,clusPos,detIdsAsInts);
  }
}


void SHEvent::addSuperCluster(const reco::SuperCluster& superClus,const SHCaloHitContainer& hits)
{
  int superClusIndx = getSuperClusIndx(superClus.rawEnergy(),superClus.position().Eta(),superClus.position().Phi()); //matches on raw energy, eta and phi in calo, returns -1 if not found
  if(superClusIndx==-1){//not already added, need to add to event
    new(superClusArray_[nrSuperClus()]) SHSuperCluster(superClus,hits);
  }
}

