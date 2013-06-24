#ifndef SHL1CALOTOWERCONTAINER_HH
#define SHL1CALOTOWERCONTAINER_HH


#include "SHarper/SHNtupliser/interface/SHL1CaloTower.hh"


#include "TClonesArray.h"

#include <vector>
#include <iostream>

class SHL1CaloTowerContainer {

 private:

  TClonesArray caloTowerArray_;
 
  mutable std::vector<int> towerIndxTable_; //! transient allows fast indexing to the hits
  static const SHL1CaloTower nullTower_;//! returns when we have an invalid id etc

 public:
  SHL1CaloTowerContainer();
  SHL1CaloTowerContainer(const SHL1CaloTowerContainer& rhs);
  virtual ~SHL1CaloTowerContainer(){}//need to check if need to delete TClonesArray objects
  
  //just uses the assignment operators of the TClonesArray 
  SHL1CaloTowerContainer& operator=(const SHL1CaloTowerContainer& rhs);

  //void addEcalHit(int detId,float nrgy);
  //  void addHcalHit(int detId,float nrgy);
  
  void addTower(const SHL1CaloTower& tower);
  
  const SHL1CaloTower& getCaloTowerByIndx(unsigned indx)const{return  *((SHL1CaloTower*) caloTowerArray_[indx]);}
  const SHL1CaloTower& getCaloTower(int iEta,int iPhi)const;
  unsigned nrCaloTowersStored()const{return caloTowerArray_.GetLast()+1;} 

  void clear(){caloTowerArray_.Delete();}
  void flushIndxTable()const{towerIndxTable_.clear();}
 private:
  //void unpackHits_()const;
  void createTowerIndxTable_()const;

  ClassDef(SHL1CaloTowerContainer,1)

};
  
#endif

  

