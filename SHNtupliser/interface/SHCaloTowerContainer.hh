#ifndef SHCALOTOWERCONTAINER
#define SHCALOTOWERCONTAINER


#include "SHarper/SHNtupliser/interface/SHCaloTower.hh"


#include "TClonesArray.h"

#include <vector>
#include <iostream>

class SHCaloTowerContainer {

 private:

  TClonesArray caloTowerArray_;
 
  mutable std::vector<int> towerIndxTable_; //! transient allows fast indexing to the hits
  static const SHCaloTower nullTower_;//! returns when we have an invalid id etc

 public:
  SHCaloTowerContainer();
  SHCaloTowerContainer(const SHCaloTowerContainer& rhs);
  virtual ~SHCaloTowerContainer(){}//need to check if need to delete TClonesArray objects
  
  //just uses the assignment operators of the TClonesArray 
  SHCaloTowerContainer& operator=(const SHCaloTowerContainer& rhs);

  //void addEcalHit(int detId,float nrgy);
  //  void addHcalHit(int detId,float nrgy);
  void addTower(int id,float emNrgy,float hadD1Nrgy,float hadD2Nrgy,float eta,float phi);
  void addTower(const SHCaloTower& tower);
  
  const SHCaloTower& getCaloTowerByIndx(unsigned indx)const{return  *((SHCaloTower*) caloTowerArray_[indx]);}
  const SHCaloTower& getCaloTower(int detId)const;
  unsigned nrCaloTowersStored()const{return caloTowerArray_.GetLast()+1;} 

  void clear(){caloTowerArray_.Delete();}
  void flushIndxTable()const{towerIndxTable_.clear();}
 private:
  //void unpackHits_()const;
  void createTowerIndxTable_()const;

  ClassDef(SHCaloTowerContainer,1)

};
  
#endif

  

