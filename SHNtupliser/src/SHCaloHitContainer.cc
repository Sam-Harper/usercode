#include "SHarper/SHNtupliser/interface/SHCaloHitContainer.hh"

#include "SHarper/SHNtupliser/interface/DetIdTools.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

ClassImp(SHCaloHitContainer)

SHCaloHitContainer::SHCaloHitContainer():
  ecalBarrelHitArray_("SHCaloHit",100000),
  ecalEndcapHitArray_("SHCaloHit",100000),
  hcalHitArray_("SHCaloHit",10000),
  hitIndxTable_()
{

}

SHCaloHitContainer::SHCaloHitContainer(const SHCaloHitContainer &rhs):
  ecalBarrelHitArray_(rhs.ecalBarrelHitArray_),
  ecalEndcapHitArray_(rhs.ecalEndcapHitArray_),
  hcalHitArray_(rhs.hcalHitArray_),
  hitIndxTable_(rhs.hitIndxTable_)
{
  
}

SHCaloHitContainer& SHCaloHitContainer::operator=(const SHCaloHitContainer& rhs)
{
  TempFuncs::copyTClonesArray<SHCaloHit>(ecalBarrelHitArray_,rhs.ecalBarrelHitArray_);
  TempFuncs::copyTClonesArray<SHCaloHit>(ecalEndcapHitArray_,rhs.ecalEndcapHitArray_);
  TempFuncs::copyTClonesArray<SHCaloHit>(hcalHitArray_,rhs.hcalHitArray_);
  hitIndxTable_ = rhs.hitIndxTable_;
  return *this;
}

void SHCaloHitContainer::addHit(int detId,float nrgy,float time,uint32_t flag,uint32_t flagBits)
{
  if(DetIdTools::isEcal(detId)){
    if(DetIdTools::isBarrel(detId)){
      new(ecalBarrelHitArray_[nrEcalBarrelHitsStored()]) SHCaloHit(detId,nrgy,time,flag,flagBits);
    }
    else if(DetIdTools::isEndcap(detId)){
      new(ecalEndcapHitArray_[nrEcalEndcapHitsStored()]) SHCaloHit(detId,nrgy,time,flag,flagBits);
    }
    else std::cout <<"SHCaloHitContainer::addHit Ecal Hit "<<std::hex<<detId<<std::dec<<" not in barrel or endcap"<<std::endl;
  }
  else if(DetIdTools::isHcal(detId)){
    new (hcalHitArray_[nrHcalHitsStored()]) SHCaloHit(detId,nrgy,time,flag,flagBits);
  }else{ 
    std::cout <<"SHCaloHitContainer::addHit "<<std::hex<<detId<<std::dec<<" not recognisefd"<<std::endl;
  }
}

const SHCaloHit& SHCaloHitContainer::getEcalBarrelHit(unsigned indx)const
{
  SHCaloHit *hit = (SHCaloHit*) ecalBarrelHitArray_[indx];
  return *hit;
}

const SHCaloHit& SHCaloHitContainer::getEcalEndcapHit(unsigned indx)const
{
  SHCaloHit *hit = (SHCaloHit*) ecalEndcapHitArray_[indx];
  return *hit;
}

void SHCaloHitContainer::addHit(const SHCaloHit& hit)
{
  addHit(hit.detId(),hit.nrgy(),hit.time(),hit.flag(),hit.flagBits());
}



float SHCaloHitContainer::getHitNrgy(int detId)const
{
  if(detId==0) return 0;
  if(DetIdTools::isEcalBarrel(detId)) return getHitNrgyEcalBarrel(detId);
  else if(DetIdTools::isEcalEndcap(detId)) return getHitNrgyEcalEndcap(detId);
  else if(DetIdTools::isHcal(detId)) return getHitNrgyHcal(detId);
  else return -999.;
}

float SHCaloHitContainer::getHitNrgyEcalBarrel(int detId)const
{
  if(detId==0) return 0;
  if(DetIdTools::isEcalBarrel(detId)){
    //if(hitIndxTable_.empty()) std::cout <<"oh shit"<<std::endl;
    if(hitIndxTable_.empty()) createHitIndxTable_(); //if we havnt already filled our nice vector for fast lookup, fill it
    int indx = hitIndxTable_[DetIdTools::getHashEcal(detId)];
    if(indx!=-1)  {
      // std::cout << "hash "<<EcalCrysPos::getArrayIndxBarrel(detId)<< " indx "<<indx<<" "<<" ecal barrel hit "<<&getEcalBarrelHit(indx)<<" size "<<ecalBarrelHitArray_.GetLast()+1<<std::endl;
return getEcalBarrelHit(indx).nrgy();
    }else return 0.;
  }else return 0.;
}

float SHCaloHitContainer::getHitNrgyEcalEndcap(int detId)const
{
  // std::cout <<" detId "<<detId<<std::endl;
  if(detId==0) return 0;
   if(DetIdTools::isEcalEndcap(detId)){
     if(hitIndxTable_.empty()) createHitIndxTable_(); //if we havnt already filled our nice vector for fast lookup, fill it
     int indx = hitIndxTable_[DetIdTools::getHashEcal(detId)];
     if(indx==-1){
       // std::cout <<"det id "<<std::hex<<detId<<std::dec<<" hash indx "<<DetIdTools::getHashEcal(detId)<<std::endl;
       // for(int i=0;i<=ecalEndcapHitArray_.GetLast();i++){
       //if(getEcalEndcapHit(i).detId()==detId){
	   //  std::cout <<"found hit at index "<<i<<" instead"<<std::endl;
       // }//else std::cout <<" hit not found i= "<<i<<std::endl;
       //}
       return 0.;
     }
     return getEcalEndcapHit(indx).nrgy(); 
     
   }else return 0.;
}

float SHCaloHitContainer::getHitNrgyHcal(int detId)const
{
  if(DetIdTools::isHcal(detId)){
    if(hitIndxTable_.empty()) createHitIndxTable_();
    int indx = hitIndxTable_[DetIdTools::getHashHcal(detId)+DetIdTools::kNrCrysBarrel+DetIdTools::kNrCrysEndcap];
    if(indx==-1){
      // std::cout <<"SHCaloHitContainer::getHitNrgyHcal: Error invalid det id "<<std::hex<<detId<<std::dec<<" hash indx "<<DetIdTools::getHashHcal(detId)+DetIdTools::kNrCrysBarrel+DetIdTools::kNrCrysEndcap<<std::endl;
      return 0.; //hit not stored
    }else if(indx>=static_cast<int>(nrHcalHitsStored())){
      std::cout <<" SHCaloHitContainer::getHitNrgyHcal : Error indx "<<indx<<" is greater or equal to number of hits stored "<<nrHcalHitsStored()<<std::endl;
      return -999.;
    }else return getHcalHitByIndx(indx).nrgy();
  }else{
    std::cout <<"SHCaloHitContainer::getHitNrgyHcal: Error det id "<<std::hex<<detId<<std::dec<<" is not in hcal"<<std::endl;
    DetIdTools::printHcalDetId(detId);
    return -999.;
  }
  
}
    

void SHCaloHitContainer::clear()
{
  ecalBarrelHitArray_.Delete();
  ecalEndcapHitArray_.Delete();
  hcalHitArray_.Delete();
  hitIndxTable_.clear();
}

//this function fills the temporary vector with a size equal to all crystals in the calorimeter
//then for each hit stored, we hash the detid to get the location it would have in the vector
//then we set that location in the vector with the index of the crystal
//this allows us to do fast constant time lookups via detid with only a moderate
//performance hit to the first loop up
void SHCaloHitContainer::createHitIndxTable_()const
{
  //first we use the swap trick to make the vector is the right size
  // and all initialised to -1
  std::vector<int> tempIndx(DetIdTools::kNrCrysBarrel+DetIdTools::kNrCrysEndcap+DetIdTools::kNrHcalCellsBarrel+DetIdTools::kNrHcalCellsEndcap,-1);
  hitIndxTable_.swap(tempIndx);
  
  for(int hitNr=0;hitNr<=ecalBarrelHitArray_.GetLast();hitNr++){
    const SHCaloHit& hit = getEcalBarrelHit(hitNr);
    int hitIndex = DetIdTools::getHashEcal(hit.detId());
    hitIndxTable_[hitIndex] = hitNr;
  }
  
  for(int hitNr=0;hitNr<=ecalEndcapHitArray_.GetLast();hitNr++){
    const SHCaloHit& hit = getEcalEndcapHit(hitNr);
    int hitIndex = DetIdTools::getHashEcal(hit.detId());
    hitIndxTable_[hitIndex] = hitNr;
  }

  for(int hitNr=0;hitNr<=hcalHitArray_.GetLast();hitNr++){
    const SHCaloHit& hit = getHcalHitByIndx(hitNr);
    int hitIndex = DetIdTools::getHashHcal(hit.detId())+DetIdTools::kNrCrysBarrel+DetIdTools::kNrCrysEndcap;
    hitIndxTable_[hitIndex] = hitNr;
  } 

}

//expands the array to the total number of hits
//so a given hit can be quickly looked up
//so strategy is to create new vectors of hit arrays with all zero energy
//then place the hits we have into these vectors at the appropriate location
//then swap the contents of the vectors with the orginal vectors
//currently only do for ecal, hcal unimplimented 
// void SHCaloHitContainer::unpackHits_()const
// {
//   SHCaloHit nullEcalHit(0,0);
//   SHCaloHit nullHcalHit(0,0);
//   std::vector<SHCaloHit> tempBarrelHitArray(DetIdTools::kNrCrysBarrel,nullEcalHit);
//   std::vector<SHCaloHit> tempEndcapHitArray(DetIdTools::kNrCrysEndcap,nullEcalHit);

//   //std::vector<SHCaloHit> tempHCalHitArray(DetId::kNrCrysHcal,nullHcalHit);

//   for(unsigned hitNr=0;hitNr<ecalBarrelHitArray_.size();hitNr++){
//     SHCaloHit& hit = ecalBarrelHitArray_[hitNr];
//     int hitIndex = EcalCrysPos::getArrayIndxBarrel(hit.detId());
//     tempBarrelHitArray[hitIndex] = hit;
//   }
  
//   for(unsigned hitNr=0;hitNr<ecalEndcapHitArray_.size();hitNr++){
//     SHCaloHit& hit = ecalEndcapHitArray_[hitNr];
//     int hitIndex = EcalCrysPos::getArrayIndxEndcap(hit.detId());
//     tempEndcapHitArray[hitIndex] = hit;
//  }

//   ecalBarrelHitArray_.swap(tempBarrelHitArray);
//   ecalEndcapHitArray_.swap(tempEndcapHitArray);
  
// }

// const SHCaloHit& SHCaloHitContainer::getEcalHit(unsigned indx)const
// {
//   SHCaloHit* theHit = NULL;
//   if(indx<nrEcalBarrelHitsStored()){
//     theHit = (SHCaloHit*) ecalBarrelHitArray_[indx];
//   }else{
//     theHit = (SHCaloHit*) ecalEndcapHitArray_[indx-nrEcalBarrelHitsStored()];
//   }
//   return *theHit;
// }

// const SHCaloHit& SHCaloHitContainer::getEcalEndcapHit(unsigned indx)const
// {
//   SHCaloHit* theHit = (SHCaloHit*) ecalEndcapHitArray_[indx];
//   return *theHit;
// }

// const SHCaloHit& SHCaloHitContainer::getEcalBarrelHit(unsigned indx)const
// {
//   SHCaloHit* theHit = (SHCaloHit*) ecalBarrelHitArray_[indx];
//   return *theHit;
// }

// const SHCaloHit& SHCaloHitContainer::getHcalHit(unsigned indx)const
// {
//   SHCaloHit* theHit = (SHCaloHit*) hcalHitArray_[indx];
//   return *theHit;
// }
