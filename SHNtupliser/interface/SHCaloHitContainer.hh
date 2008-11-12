#ifndef SHECALOHITCONTAINER
#define SHECALOHITCONTAINER

//class is a root storable object which contains a "domino" of ecal hits
//which are at constant phi but variable eta

//WARNING, when root loads the object in from the file, it doesnt override the transient members, ie the hitIndxTable remains unmodified. So for now, when you load the object in from file, you need to clear the hitIndx. Working on automating this with a custom streamer



#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"


#include "TObject.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <vector>
#include <iostream>

class SHCaloHitContainer : public TObject {

 private:

  //horrible bandaid on design, you see I modify these in the unpacking but I dont actually change any information...
  //it might be better to create a temporay vector which either holds copies of the hits or the indexes in the relavent vector
  TClonesArray ecalBarrelHitArray_;
  TClonesArray ecalEndcapHitArray_;
  TClonesArray hcalHitArray_;

  mutable std::vector<int> hitIndxTable_; //! transient allows fast indexing to the hits
 

  //SHEcalHit nullEcalHit_; //! returned when the ecal hit isnt found
  //SHHcalHit nullHcalHit_; //! returned when the hcal hit isnt found

 public:
  SHCaloHitContainer();
  SHCaloHitContainer(const SHCaloHitContainer& rhs);
  ~SHCaloHitContainer(){}//need to check if need to delete TClonesArray objects
  
  //just uses the assignment operators of the TClonesArray and vector
  //decided to copy the vector accross as well
  SHCaloHitContainer& operator=(const SHCaloHitContainer& rhs);

  //void addEcalHit(int detId,float nrgy);
  //  void addHcalHit(int detId,float nrgy);
  void addHit(int detId,float nrgy);
  void addHit(const SHCaloHit& hit);
  

  const SHCaloHit& getEcalBarrelHit(unsigned indx)const;
  const SHCaloHit& getEcalEndcapHit(unsigned indx)const;
  // const SHHcalHit& getHcalhit(unsigned indx)const;

  float getHitNrgy(int detId)const;
  float getHitNrgyEcalBarrel(int detId)const;
  float getHitNrgyEcalEndcap(int detId)const;
  float getHitNrgyHcal(int detId)const;

  const SHCaloHit& getEcalBarrelHitByIndx(unsigned indx)const{return *((SHCaloHit*) ecalBarrelHitArray_[indx]);}
  const SHCaloHit& getEcalEndcapHitByIndx(unsigned indx)const{return *((SHCaloHit*) ecalEndcapHitArray_[indx]);}
  const SHCaloHit& getHcalHitByIndx(unsigned indx)const{return  *((SHCaloHit*) hcalHitArray_[indx]);}
  const SHCaloHit& getEcalHitByIndx(unsigned indx)const{return indx<nrEcalBarrelHitsStored() ? getEcalBarrelHitByIndx(indx) : getEcalEndcapHitByIndx(indx-nrEcalBarrelHitsStored());}

  unsigned nrEcalHitsStored()const{return nrEcalBarrelHitsStored()+nrEcalEndcapHitsStored();}
  unsigned nrHitsStored()const{return nrEcalHitsStored()+nrHcalHitsStored();}
  unsigned nrEcalBarrelHitsStored()const{return ecalBarrelHitArray_.GetLast()+1;}
  unsigned nrEcalEndcapHitsStored()const{return ecalEndcapHitArray_.GetLast()+1;} 
  unsigned nrHcalHitsStored()const{return hcalHitArray_.GetLast()+1;} 

  void clear();

  void flushIndxTable()const{hitIndxTable_.clear();}

 private:
  //void unpackHits_()const;
  void createHitIndxTable_()const;

  ClassDef(SHCaloHitContainer,3)

};
  
#endif

  

