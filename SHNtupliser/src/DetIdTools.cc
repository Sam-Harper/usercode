#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "SHarper/SHNtupliser/interface/LogErr.hh"

#include<cstdlib>
#include<cmath>




DetIdTools::EcalNavigator::EcalNavigator(int startId):
  startId_(startId)
{ 
  if(DetIdTools::isEcalBarrel(startId)) isBarrel_=true;
  else if(DetIdTools::isEcalEndcap(startId)) isBarrel_=false;
  else{
    isBarrel_=false;
    LogErr <<" Error "<<startId<<" is not ecal barrel or endcap, navigation is undefined"<<std::endl;
    startId_=0;
  }
 
  initPosCoords_();
 
}

void DetIdTools::EcalNavigator::initPosCoords_()
{
  if(isBarrel_) {
    currEtaOrX_= iEtaBarrel(startId_);
    currPhiOrY_= iPhiBarrel(startId_);
    zSide_ = 0;
  }else{
    currEtaOrX_ = iXEndcap(startId_);
    currPhiOrY_ = iYEndcap(startId_);
    zSide_ = zEndcap(startId_); 
  }
}

int DetIdTools::EcalNavigator::goNorth(int nrSteps)
{
  currPhiOrY_+=nrSteps;
  return curPos();
}

int DetIdTools::EcalNavigator::goEast(int nrSteps)
{
  currEtaOrX_+=nrSteps;
  //little fix in barrel as there is no crystal at eta =0;
  if(isBarrel_ && currEtaOrX_==0){
    if(nrSteps>0) currEtaOrX_++;
    else if(nrSteps<0) currEtaOrX_--;
  }
    
  return curPos();
}

int DetIdTools::EcalNavigator::curPos()const
{
  return getDetId_(currEtaOrX_,currPhiOrY_);
}

int DetIdTools::EcalNavigator::getIdAtPos(int nrStepsEast,int nrStepsNorth)const   
{
  int posEtaOrX = currEtaOrX_+nrStepsEast;
  if(isBarrel_ && posEtaOrX*currEtaOrX_<=0){ //little fix for barrel having no crystal at 0, going directly from -1 to 1
    if(currEtaOrX_<0) posEtaOrX++;
    else posEtaOrX--;
  }
  int posPhiOrY = currPhiOrY_+nrStepsNorth;
  while(isBarrel_ && posPhiOrY>360) posPhiOrY-=360;
  while(isBarrel_ && posPhiOrY<=0) posPhiOrY+=360;
  return getDetId_(posEtaOrX,posPhiOrY);

}

int DetIdTools::EcalNavigator::getDetId_(int etaOrX,int phiOrY)const
{
  if(isBarrel_){
    if(isValidEcalBarrelId(etaOrX,phiOrY)){
      return makeEcalBarrelId(etaOrX,phiOrY);
    }else return 0;
  }else{
    // std::cout <<"endcap id for x "<<etaOrX<<" ("<<currEtaOrX_<<")"<<" "<<phiOrY<<" ("<<currPhiOrY_<<")";
    if(isValidEcalEndcapId(etaOrX,phiOrY,zSide_)){
      //std::cout <<" is valid, id is "<<makeEcalEndcapId(etaOrX,phiOrY,zSide_)<<std::endl;
      return makeEcalEndcapId(etaOrX,phiOrY,zSide_);
    }else{
      //std::cout <<"is not valid "<<std::endl;
      return 0;
    }
  }
}


//magic numbers stolen from CMSSW EEDetId
const int DetIdTools::nBegin_[kIXMax_] = { 41, 41, 41, 36, 36, 26, 26, 26, 21, 21, 21, 21, 21, 16, 16, 14, 14, 14, 14, 14, 9, 9, 9, 9, 9, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 14, 14, 14, 14, 14, 16, 16, 21, 21, 21, 21, 21, 26, 26, 26, 36, 36, 41, 41, 41 };
const int DetIdTools::nIntegral_[kIXMax_] = { 0, 20, 40, 60, 90, 120, 170, 220, 270, 330, 390, 450, 510, 570, 640, 710, 784, 858, 932, 1006, 1080, 1164, 1248, 1332, 1416, 1500, 1590, 1680, 1770, 1860, 1950, 2040, 2130, 2220, 2310, 2400, 2494, 2588, 2682, 2776, 2870, 2970, 3070, 3170, 3270, 3370, 3470, 3570, 3670, 3770, 3870, 3970, 4070, 4170, 4270, 4370, 4470, 4570, 4670, 4770, 4870, 4964, 5058, 5152, 5246, 5340, 5430, 5520, 5610, 5700, 5790, 5880, 5970, 6060, 6150, 6240, 6324, 6408, 6492, 6576, 6660, 6734, 6808, 6882, 6956, 7030, 7100, 7170, 7230, 7290, 7350, 7410, 7470, 7520, 7570, 7620, 7650, 7680, 7700, 7720 };

DetIdTools::DetIdTools()
{

}

int DetIdTools::makeEcalBarrelId(int iEta,int iPhi)
{
 
  int iEtaAbs = std::abs(iEta);
  int detId=(kEcalCode | kBarrelCode);
  detId |= (iEtaAbs<<9);
  detId |= iPhi;
  if(iEta>0) detId |= 0x10000;
  return detId;

}

int DetIdTools::makeEcalEndcapId(int ix,int iy,int iz)
{
  int detId=(kEcalCode | kEndcapCode);
  detId |= (iy&0x7F);
  detId |= ((ix&0x7F)<<7);
  detId |= ((iz>0)?(0x4000):(0));
  return detId;
}

// int DetIdTools::getHashEcal(int detId)
// {
//   if(isEcal(detId)){
//     if(isBarrel(detId)) return getHashEcalBarrel(detId);
//     else if(isEndcap(detId)) return getHashEcalEndcap(detId)+kNrCrysBarrel;
//   }
//   //not ecal barrel or endcap
//   std::cout <<"DetIdTools::getHashEcal, detId "<<std::hex<<detId<<std::dec<<" is invalid"<<std::endl;
//   return -1;
// }


// int DetIdTools::getHashEcalBarrel(int detId)
// {
//   int etaBandNr =  kMaxIEtaBarrel + (positiveZBarrel(detId) ? iEtaAbsBarrel(detId)-1 : -iEtaAbsBarrel(detId)); // 0 - 189 starting at lowest eta (~-1.5) to highest
//   return etaBandNr* kMaxIPhiBarrel + iPhiBarrel(detId)-1;
// }


// int DetIdTools::getHashEcalEndcap(int detId)
// {
//   return iYEndcap(detId) - nBegin_[iXEndcap(detId)-1] + nIntegral_[iXEndcap(detId) -1 ] + (positiveZEndcap(detId) ? kICrFee_ : 0);
// }
  
int DetIdTools::makeHcalDetId(int subDetCode,int iEta,int iPhi,int depth)
{
  int detId=0x0;
  detId |= kHcalCode;
  detId |= subDetCode;
  detId |= (iPhi&0x7F);
  if(iEta>0) detId |= (0x2000 | (iEta<<7) );
  else detId |= ((-iEta)<<7);
  detId |= ((depth&0x7)<<14);
  return detId;
}

void DetIdTools::printHcalDetId(int detId)
{
  std::cout <<"detId "<<std::hex<<detId<<std::dec<<" isHcal "<<isHcal(detId)<<" isBarrel "<<isBarrel(detId)<<" isEndcap "<<isEndcap(detId)<<" iEta "<<iEtaHcal(detId)<<" iPhi "<<iPhiHcal(detId)<<" depth "<<depthHcal(detId)<<std::endl;
}

//ported from HcalDetId.hash_index in CMSSW
int DetIdTools::getHashHcal(int detId)
{
  if(!DetIdTools::isHcal(detId)){
    std::cout <<"DetIdTools::getHashHcal: Warning det id "<<std::hex<<detId<<std::dec<<" is not in the hcal"<<std::endl;
    return -1;
  }
  
  int index = -1;

  int HBhalf = kNrHcalCellsBarrel/2;
  int HEhalf = kNrHcalCellsEndcap/2;
  
  int iPhi = DetIdTools::iPhiHcal(detId);
  int iEtaAbs = DetIdTools::iEtaAbsHcal(detId);
  int zSide = DetIdTools::zSideHcal(detId);
  int depth = DetIdTools::depthHcal(detId);

  // HB valid DetIds: phi=1-72,eta=1-14,depth=1; phi=1-72,eta=15-16,depth=1-2
  if (DetIdTools::isHcalBarrel(detId)) {
    if (iEtaAbs < 16)   index = (iPhi - 1)*18 + (iEtaAbs - 1) + (depth - 1);
    if (iEtaAbs == 16)  index = (iPhi - 1)*18 + iEtaAbs + (depth - 1);
    
    if (zSide == -1) index += HBhalf;
    // HE valid DetIds: phi=1-72,eta=16-17,depth=1; phi=1-72,eta=18-20,depth=1-2; 
  //                  phi=1-71(in steps of 2),eta=21-26,depth=1-2; phi=1-71(in steps of 2),eta=27-28,depth=1-3
  //                  phi=1-71(in steps of 2),eta=29,depth=1-2
    
  } else if (DetIdTools::isHcalEndcap(detId)) {
    if (iEtaAbs == 16 || iEtaAbs == 17)  index = (iPhi - 1)*8 + (iPhi/2)*20 + (iEtaAbs - 16);
    if (iEtaAbs >= 18 && iEtaAbs <= 20)  index = (iPhi - 1)*8 + (iPhi/2)*20 + 2  + 2*(iEtaAbs-18) + (depth - 1);
    if (iEtaAbs >= 21 && iEtaAbs <= 26)  index = (iPhi - 1)*8 + (iPhi/2)*20 + 8  + 2*(iEtaAbs-21) + (depth - 1);
    if (iEtaAbs >= 27 && iEtaAbs <= 28)  index = (iPhi - 1)*8 + (iPhi/2)*20 + 20 + 3*(iEtaAbs-27) + (depth - 1);
    if (iEtaAbs == 29)                     index = (iPhi - 1)*8 + (iPhi/2)*20 + 26 + 2*(iEtaAbs-29) + (depth - 1);
    
    index += 2*HBhalf;
    if (zSide == -1) index += HEhalf;
  }
  
  return index;
}

//you know, using the info in SHCaloGeom, I could achieve this faster and in half the lines in one loop
//but it somehow doesnt feel right going there
//if any entry has zero, its a wildcard, to select all cells in tower 20, on both sides at depth 2, do
//getMatchingIdsHcal(20,0,0,2,ids)
void DetIdTools::getMatchingIdsHcal(int etaAbs,int phi,int side,int depth,std::vector<int>& ids)
{
  for(int etaAbsNr=kHcalIEtaAbsMin;etaAbsNr<=kHcalIEtaAbsMax;etaAbsNr++){
    for(int phiNr=kHcalIPhiMin;phiNr<kHcalIPhiMax;phiNr++){
      for(int depthNr=kHcalDepthMin;depthNr<=kHcalDepthMax;depthNr++){
	for(int sideNr=-1;sideNr<=1;sideNr+=2){
	  if(isValidHcalId(etaAbsNr*sideNr,phiNr,depthNr)){
	    if( (etaAbsNr==etaAbs || etaAbs==0) && //eta match
		(phiNr==phi || phi==0) && //phi match
		(depthNr==depth || depth==0) && //depth match
		(sideNr==side || side==0) ) { //side match
	      int subDet=0;
	      if(etaAbsNr<=15 || (etaAbsNr==16 && depthNr<3)) subDet=kBarrelCode;
	      else subDet=kEndcapCode;
	      ids.push_back(makeHcalDetId(subDet,etaAbsNr*sideNr,phiNr,depthNr));
	    }//end of cell match check
	  }//end of valid hcal det id check
	}//end of side loop
      }//end of depth loop
    }//end of phi loop
  }//end of eta loop
}

bool DetIdTools::isValidHcalId(int iEta,int iPhi,int depth)
{
  return isValidHcalBarrelId(iEta,iPhi,depth) || isValidHcalEndcapId(iEta,iPhi,depth);
}

//depth 1= all towers up to and including tower 17 + depth 1 18-29 + depth 2 27-29
//depth 2= depth 2 18-26, depth 3 27-29
//warning, emulating calo tower bug where depth 2 tower 27 is depth 2 and depth 3 is depth 1
int DetIdTools::getEffectiveHcalDepth(int detId)
{
  if(!isHcal(detId)){
    LogErr <<" : Warning detId "<<detId<<" is not hcal "<<std::endl;
    return 0;
  }else{
    int iEtaAbs = iEtaAbsHcal(detId);
    int depth = depthHcal(detId);
    if(iEtaAbs<=17 || 
       (iEtaAbs<=29 && depth==1) ||
       // (iEtaAbs>=27 && iEtaAbs<=29 && depth==2)){   
       (iEtaAbs>=28 && iEtaAbs<=29 && depth==2) || 
       (iEtaAbs==27 && depth==3)){
      return 1;
    }else return 2;
  }
}  

//towers with one depth 1-15,17
//towers with two depths 18-26
//towers with three depths 16,27-29 (
int DetIdTools::getNrDepthsInHcalTower(int detId)
{
  if(!isHcal(detId)){
    LogErr <<" : Warning detId "<<detId<<" is not hcal,not implimented yet for ecal ids "<<std::endl;
    return 0;
  }else{
    int iEtaAbs = iEtaAbsHcal(detId);
    if(iEtaAbs<=15) return 1;
    else if(iEtaAbs==16) return 3;
    else if(iEtaAbs==17) return 1;
    else if(iEtaAbs<=26) return 2;
    else if(iEtaAbs<=29) return 3;
    else {
      LogErr <<" : Warning detId "<<detId<<" has invalid abs eta "<<iEtaAbs<<std::endl;
      return -1;
    }
  }//end valid hcal det id check
}

bool DetIdTools::isValidHcalBarrelId(int iEta,int iPhi,int depth)
{
  const int hcalBarrelIEtaMax = 16;
  const int hcalBarrelIPhiMin = 1;
  const int hcalBarrelIPhiMax = 72;
  
  if(abs(iEta)<=hcalBarrelIEtaMax){ //iEta good
    if(iPhi>=hcalBarrelIPhiMin && iPhi<=hcalBarrelIPhiMax){ //iPhi good
      if(depth==1 || (abs(iEta)>=15 && depth==2)){ //depth good
	return true;
      }
    }
  }
  return false;
}

bool DetIdTools::isValidHcalEndcapId(int iEta,int iPhi,int depth)
{
  const int iEtaMin = 16;
  const int iEtaMax = 29;
  const int iEtaPhiBoundary=21;
  const int iPhiMin = 1;
  const int iPhiMax = 72;// 


  //first check ieta and phi 
  if(abs(iEta)<iEtaMin || abs(iEta)>iEtaMax) return false;
  else if(iPhi<iPhiMin || iPhi>iPhiMax) return false;

  //final phi check, if its above the eta boundary, it goes like 1,3,5,..,69,71
  //so we check it isnt odd
  if(abs(iEta)>=iEtaPhiBoundary && iPhi%2==0) return false;

  //depth checks
  if(abs(iEta)==16){
    if(depth==3) return true;
    else return false;
  }else if(abs(iEta)==17){
    if(depth==1) return true;
    else return false;
  } else if(abs(iEta)<=26){
    if(depth>=1 && depth<=2) return true;
    else return false;
  }else if(abs(iEta)<=28){
    if(depth>=1 && depth<=3) return true;
    else return false;
  }else if(abs(iEta)<=29){
    if(depth>=1 && depth<=2) return true;
    else return false;
  }else return false;
}

bool DetIdTools::isValidEcalBarrelId(int iEta, int iPhi) 
{
  bool valid = true;
  if (iEta < -kMaxIEtaBarrel || iEta == 0 || iEta > kMaxIEtaBarrel ||
      iPhi < kMinIPhiBarrel || iPhi > kMaxIPhiBarrel) {
    valid = false;
  }  
  return valid;

}
//stolen from EEDetId.cc in CMSSW
bool DetIdTools::isValidEcalEndcapId(int crystal_ix,int crystal_iy,int iz)
{
  bool valid = false;
  if (crystal_ix < kMinIXEndcap || crystal_ix > kMaxIXEndcap ||
      crystal_iy < kMinIYEndcap || crystal_iy > kMaxIYEndcap || abs(iz) != 1 ) 
    { 
      return valid ; 
    }
  if ( (crystal_ix >= 1 && crystal_ix <= 3 && (crystal_iy <= 40 || crystal_iy > 60) ) ||
       (crystal_ix >= 4 && crystal_ix <= 5 && (crystal_iy <= 35 || crystal_iy > 65) ) || 
       (crystal_ix >= 6 && crystal_ix <= 8 && (crystal_iy <= 25 || crystal_iy > 75) ) || 
       (crystal_ix >= 9 && crystal_ix <= 13 && (crystal_iy <= 20 || crystal_iy > 80) ) || 
       (crystal_ix >= 14 && crystal_ix <= 15 && (crystal_iy <= 15 || crystal_iy > 85) ) || 
       (crystal_ix >= 16 && crystal_ix <= 20 && (crystal_iy <= 13 || crystal_iy > 87) ) || 
       (crystal_ix >= 21 && crystal_ix <= 25 && (crystal_iy <= 8 || crystal_iy > 92) ) || 
       (crystal_ix >= 26 && crystal_ix <= 35 && (crystal_iy <= 5 || crystal_iy > 95) ) || 
       (crystal_ix >= 36 && crystal_ix <= 39 && (crystal_iy <= 3 || crystal_iy > 97) ) || 
       (crystal_ix >= 98 && crystal_ix <= 100 && (crystal_iy <= 40 || crystal_iy > 60) ) ||
       (crystal_ix >= 96 && crystal_ix <= 97 && (crystal_iy <= 35 || crystal_iy > 65) ) || 
       (crystal_ix >= 93 && crystal_ix <= 95 && (crystal_iy <= 25 || crystal_iy > 75) ) || 
       (crystal_ix >= 88 && crystal_ix <= 92 && (crystal_iy <= 20 || crystal_iy > 80) ) || 
       (crystal_ix >= 86 && crystal_ix <= 87 && (crystal_iy <= 15 || crystal_iy > 85) ) || 
       (crystal_ix >= 81 && crystal_ix <= 85 && (crystal_iy <= 13 || crystal_iy > 87) ) || 
       (crystal_ix >= 76 && crystal_ix <= 80 && (crystal_iy <= 8 || crystal_iy > 92) ) || 
       (crystal_ix >= 66 && crystal_ix <= 75 && (crystal_iy <= 5 || crystal_iy > 95) ) || 
       (crystal_ix >= 62 && crystal_ix <= 65 && (crystal_iy <= 3 || crystal_iy > 97) ) ||
       ( (crystal_ix == 40 || crystal_ix == 61) && ( (crystal_iy >= 46 && crystal_iy <= 55 ) || crystal_iy <= 3 || crystal_iy > 97 )) ||
       ( (crystal_ix == 41 || crystal_ix == 60) && crystal_iy >= 44 && crystal_iy <= 57 ) ||
       ( (crystal_ix == 42 || crystal_ix == 59) && crystal_iy >= 43 && crystal_iy <= 58 ) ||
       ( (crystal_ix == 43 || crystal_ix == 58) && crystal_iy >= 42 && crystal_iy <= 59 ) ||
       ( (crystal_ix == 44 || crystal_ix == 45 || crystal_ix == 57 || crystal_ix == 56) && crystal_iy >= 41 && crystal_iy <= 60 ) ||
       ( crystal_ix >= 46 && crystal_ix <= 55 && crystal_iy >= 40 && crystal_iy <= 61 ) 
       )
    { 
      return valid; 
    }
  valid = true;
  return valid;
  
}


int DetIdTools::endcapEtaRing(int detId)
{
  if(DetIdTools::isEcalEndcap(detId)){
    return endcapEtaRing(DetIdTools::iXEndcap(detId),DetIdTools::iYEndcap(detId));
  }else return -1;
}


//there are 40 rings in eta (i think)
int DetIdTools::endcapEtaRing(int ix,int iy)
{
  if(DetIdTools::isValidEcalEndcapId(ix,iy,1)){
    int iXNorm  = DetIdTools::normEndcapIXOrIY(ix);
    int iYNorm  = DetIdTools::normEndcapIXOrIY(iy);
    float iR = std::sqrt(iXNorm*iXNorm+iYNorm*iYNorm);

    int iRInt = static_cast<int>(iR);
    if(iR-iRInt>0.5) iRInt++; //rounding up

    return iRInt-10;
  }else return -1;
}

float DetIdTools::endcapEtaRingFloat(int ix,int iy)
{
  if(DetIdTools::isValidEcalEndcapId(ix,iy,1)){
    int iXNorm  = DetIdTools::normEndcapIXOrIY(ix);
    int iYNorm  = DetIdTools::normEndcapIXOrIY(iy);
    float iR = std::sqrt(iXNorm*iXNorm+iYNorm*iYNorm);
    
    return iR;//-10.;
  }else return -1;
}

float DetIdTools::endcapEtaRingFloat(int detId)
{
  if(DetIdTools::isEcalEndcap(detId)){
    return endcapEtaRingFloat(DetIdTools::iXEndcap(detId),DetIdTools::iYEndcap(detId));
  }else return -1.;
}

int DetIdTools::normEndcapIXOrIY(int iXOrIY)
{
  int iXOrIYNorm = iXOrIY -50;
  //want to map 1=-50,50=-1,51=1 and 100 to 50 so sub off one if zero or neg
  if(iXOrIYNorm<=0) iXOrIYNorm--;
  return iXOrIYNorm;

}
