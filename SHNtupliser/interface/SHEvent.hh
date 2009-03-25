#ifndef SHEVENT
#define SHEVENT

//Event class which stores all the objects in the event
//currently limited to superclusters + electrons

#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHElectron.hh"
#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"
#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"
#include "SHarper/SHNtupliser/interface/SHJet.hh"
#include "SHarper/SHNtupliser/interface/SHIsolCluster.hh"
#include "SHarper/SHNtupliser/interface/SHIsolSuperCluster.hh"
#include "SHarper/SHNtupliser/interface/SHIsolTrack.hh"
#include "SHarper/SHNtupliser/interface/SHTrigInfo.hh"
#include "SHarper/SHNtupliser/interface/SHMet.hh"

#include "TObject.h"
#include "TClonesArray.h"

#include <map>
#include <vector>

namespace reco{
  class SuperCluster;
  class BasicCluster;
}

namespace pat{
  class Jet;
}

namespace heep{
  class Ele;
}

class SHEvent : public TObject {

 private:
  TClonesArray superClusArray_;
  TClonesArray electronArray_;
  TClonesArray mcPartArray_;
  TClonesArray jetArray_; //new for v4
  SHCaloHitContainer caloHits_; //new for v4
  TClonesArray isolSuperClusArray_;//new for v4
  TClonesArray isolClusArray_;//new for v4
  TClonesArray isolTrkArray_;//new for v5
  TClonesArray trigArray_;

 
  int runnr_;
  int eventnr_;
  bool isMC_;
  int datasetCode_;
  double weight_;
  SHMet metData_; //new for v9
  double genEventPtHat_; 

  SHEvent(const SHEvent &rhs){}//disabling copying for now
  SHEvent& operator=(const SHEvent& rhs){return *this;}//disabling assignment

 public:
  SHEvent();
  ~SHEvent();//need to check if need to delete TClonesArray objects

  //note: change pointers to reference variables
  //shapeMap : all the clusters + shape making up the cluster with seed 1st
  //dominoVec : all the dominos starting at -phi to +phi with seed in middle

  void addElectron(const heep::Ele& ele,const SHCaloHitContainer& hits);
  void addElectron(const SHElectron& ele,const SHSuperCluster& superClus);
  void addElectron(const SHElectron& ele);
  void addJet(const pat::Jet& jet);
  void addJet(const SHJet& jet);
  void addSuperCluster(const reco::SuperCluster& superClus,const SHCaloHitContainer& hits);
  void addIsolCluster(const reco::BasicCluster& clus);
  void addIsolSuperCluster(const reco::SuperCluster& superClus);
  void addIsolTrk(const SHIsolTrack& trk);
  void addIsolTrk(const TVector3& p3,const TVector3& vtxPos,bool posCharge);
 
  void addMCParticle(int partIndx,int partStdhep,int partIdhep,
		     int partJmo1,int partJmo2,int partNrMo,
		     int partJda1,int partJda2,int partNrDa,
		     TLorentzVector& p4);
  void addEcalHits(const std::vector<SHCaloHit> & hitVec);
  void addHcalHits(const std::vector<SHCaloHit> & hitVec);  
  void addMCParticle(const SHMCParticle& mcPart);
  void addTrigInfo(const SHTrigInfo& trigInfo);

  //usefull for copying an event
  void addCaloHits(const SHEvent& rhs);
  void addIsolInfo(const SHEvent& rhs);
  
  void setRunnr(int runnr){runnr_=runnr;}
  void setEventnr(int eventnr){eventnr_=eventnr;}
  void setIsMC(bool isMC){isMC_=isMC;}
  void setDatasetCode(int datasetCode){datasetCode_=datasetCode;}
  void setWeight(double weight){weight_=weight;}
  void setMet(const SHMet& met){metData_=met;}
  void setGenEventPtHat(double ptHat){genEventPtHat_=ptHat;}
  void copyEventPara(const SHEvent& rhs);
  void clear();


  const SHSuperCluster* getSuperClus(int clusNr)const;
  const SHMCParticle* getMCParticle(int partNr)const;
  const SHElectron* getElectron(int eleNr)const;
  const SHIsolCluster* getIsolClus(int clusNr)const;
  const SHIsolSuperCluster* getIsolSuperClus(int clusNr)const;
  const SHJet* getJet(int jetNr)const;
  SHJet* getJet(int jetNr);

  const SHIsolTrack* getIsolTrk(int trkNr)const;
  SHElectron* getElectron(int eleNr); //allows the event to modify the electron
  const SHTrigInfo* getTrigInfo(int trigNr)const;

  int getSuperClusIndx(float rawNrgy,float eta,float phi)const; //exactly matches based on energy, calorimeter eta, phi returns -1 if not found
  int getIsolClusIndx(float rawNrgy,float eta,float phi)const; //exactly matches based on energy, calorimeter eta, phi returns -1 if not found

  int nrSuperClus()const{return superClusArray_.GetLast()+1;}
  int nrElectrons()const{return electronArray_.GetLast()+1;}
  int nrMCParticles()const{return mcPartArray_.GetLast()+1;}
  int nrJets()const{return jetArray_.GetLast()+1;}
  int nrIsolClus()const{return isolClusArray_.GetLast()+1;}
  int nrIsolSuperClus()const{return isolSuperClusArray_.GetLast()+1;}
  int nrIsolTrks()const{return isolTrkArray_.GetLast()+1;}
  int nrTrigs()const{return trigArray_.GetLast()+1;}

  int runnr()const{return runnr_;}
  int eventnr()const{return eventnr_;}
  int datasetCode()const{return datasetCode_;}
  float weight()const{return weight_;}
  const SHMet& metData()const{return metData_;}
  SHMet& metData(){return metData_;}
  bool isMC()const{return isMC_;}
  const SHCaloHitContainer& getCaloHits()const{return caloHits_;}
  double genEventPtHat(){return genEventPtHat_;}

  //first function gets the triggers passed for the event
  //second to functions get the triggers passed for a particlar object
  int getTrigCode()const;
  int getTrigCode(const TLorentzVector& p4){return getTrigCode(p4.Eta(),p4.Phi());}
  int getTrigCode(double eta,double phi);

  //some objects have a temporary transisent data cache which root doesnt
  //override when it fills them
  //so it needs to be manually cleared (very annoying)
  void flushTempData()const;
  
  //a tempory function, this removes all the duplicate electrons in the event
  void removeDupEles(std::vector<int>& dupEleNrs);

  void printTruth(int nrLines=-1)const;

 private:
  
  SHSuperCluster* getSuperClus_(int clusNr); //allows the event to modify the electron

  ClassDef(SHEvent,10) //5 is v3, 6 is v4

};
  
  
#endif
