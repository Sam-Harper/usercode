#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"

#include "SHarper/SHNtupliser/interface/DetIdTools.hh"
#include "SHarper/SHNtupliser/interface/MathFuncs.hh"

ClassImp(SHCaloGeom)

const SHCaloCellGeom SHCaloGeom::nullCell_;

SHCaloGeom::SHCaloGeom(int detCode):
  detCode_(detCode),
  cellGeoms_()
{
  //now resize the vector to have enough elements to hold the relavent calo geom
  if(detCode_==ECAL) cellGeoms_.resize(DetIdTools::kNrEcalCellsBarrel+DetIdTools::kNrEcalCellsEndcap);
  else if(detCode_==HCAL) cellGeoms_.resize(DetIdTools::kNrHcalCellsBarrel+DetIdTools::kNrHcalCellsEndcap);
  else std::cout <<"SHCaloGeom::SHCaloGeom : Error detCode "<<detCode<<" is not ECAL or HCAL"<<std::endl;
}

//getEcalArrayIndx,getHcalArrayIndx automatically check that the det id is in correct det
void SHCaloGeom::setCellPos(int detId,const TVector3& pos,int towerId)
{
  int indx=-2;
  if(detCode_==ECAL) indx = DetIdTools::getHashEcal(detId);
  else if(detCode_==HCAL) indx = DetIdTools::getHashHcal(detId);
  if(indx>=0 && indx<(int)cellGeoms_.size()) cellGeoms_[indx].setCellGeom(detId,pos,towerId);
  else std::cout <<"SHCaloGeom::addCell : Warning indx "<<indx<<" is invalid, detid "<<std::hex<<detId<<std::dec<<" detCode "<<detCode_<<std::endl;
}

void SHCaloGeom::setCellPos(int detId,const TVector3& pos,int towerId,
			    const SHCaloCellGeom::CellEdges& front,
			    const SHCaloCellGeom::CellEdges& rear)
{
  int indx=-2;
  if(detCode_==ECAL) indx = DetIdTools::getHashEcal(detId);
  else if(detCode_==HCAL) indx = DetIdTools::getHashHcal(detId);
  if(indx>=0 && indx<(int)cellGeoms_.size()) cellGeoms_[indx].setCellGeom(detId,pos,towerId,front,rear);
  else std::cout <<"SHCaloGeom::addCell : Warning indx "<<indx<<" is invalid, detid "<<std::hex<<detId<<std::dec<<" detCode "<<detCode_<<std::endl;
}


// //getEcalArrayIndx,getHcalArrayIndx automatically check that the det id is in correct det
// const SHCaloCellGeom& SHCaloGeom::getCell(int detId)const
// {
//   int indx=-2;
//   if(detCode_==ECAL) indx = DetIdTools::getHashEcal(detId);
//   else if(detCode_==HCAL) indx = DetIdTools::getHashHcal(detId);
//   if(indx>=0 && indx<(int)cellGeoms_.size())  return cellGeoms_[indx];
//   else {
//     std::cout <<"SHCaloGeom::getCell : Warning indx "<<indx<<" is invalid, detid "<<std::hex<<detId<<std::dec<<" detCode "<<detCode_<<std::endl;
//     return nullCell_;
//   }
// }

const SHCaloCellGeom& SHCaloGeom::getClosestCell(double centreEta,double centrePhi)const
{
  
  double minDeltaR2 = 9999999.;
  int closestCellNr=-1;

 
  for(size_t cellNr=0;cellNr<cellGeoms_.size();cellNr++){
    const SHCaloCellGeom& cell = cellGeoms_[cellNr];
    if(detCode_==HCAL && DetIdTools::depthHcal(cell.detId())>1) continue; //skip hcal cells which dont have depth 1
    double deltaR2 = MathFuncs::calDeltaR2(centreEta,centrePhi,cell.eta(),cell.phi());
    if(deltaR2<minDeltaR2){
      minDeltaR2 = deltaR2;
      closestCellNr=cellNr;
    }
  }
  
  if(closestCellNr!=-1) return cellGeoms_[closestCellNr];
  else{
    std::cout <<"SHCaloGeom::getClosestCell : Warning, no cell close to eta "<<centreEta<<" phi "<<centrePhi<<std::endl;
    return nullCell_;
  }

}

//this really only makes sense for hcal geometries but will gracefull return
//a vector of one for the ecal
//the hcal has multiple depths which is why we need this function
//it also has complications for tower 16 and 28/29
//tower 16 is both in the barrel and endcap so need to handle this specially
//towers 28/29 combine into tower 28 in the 3rd depth, that is the first two segmentments
//tower "28" are split into two in eta, so this is also a special case
std::vector<SHCaloCellGeom> SHCaloGeom::getCellsInEtaPhiTower(int detId)const
{
  //first create the return vector
  std::vector<SHCaloCellGeom> cellList;
 
  //next check if in ecal, if so our job is easy
  if(detCode_==ECAL) {
    cellList.push_back(getCell(detId));
    return cellList;
  }else if(detCode_==HCAL){
    //now the function proper starts, first get the tower iEta
    int iEtaAbs = DetIdTools::iEtaAbsHcal(detId);
    int iPhi = DetIdTools::iPhiHcal(detId);
    int iEta = DetIdTools::iEtaHcal(detId);
    if(iEtaAbs<=14 || iEtaAbs==17) { //these towers only have one depth
      cellList.push_back(getCell(detId));
      return cellList;
    }else if(iEtaAbs==15 || (iEtaAbs>=18 && iEtaAbs<=28)){ //these are towers with two or three depths (27,28 has 3 others have 2)
      int nrDepths=2;
      if(iEtaAbs>=27) nrDepths=3;
      for(int depthNr=1;depthNr<=nrDepths;depthNr++){
	int cellDetId = 0;
	//now we have the slight complication that 15 is barrel while the others are endcap
	if(iEtaAbs==15) cellDetId = DetIdTools::makeHcalBarrelDetId(iEta,iPhi,depthNr);
	else cellDetId = DetIdTools::makeHcalEndcapDetId(iEta,iPhi,depthNr);
	cellList.push_back(getCell(cellDetId));
      }
      return cellList;
    }else if(iEtaAbs==16){ //joy first special case
      //first do barrel parts of tower 16
      for(int depthNr=1;depthNr<=2;depthNr++){
	int cellDetId = DetIdTools::makeHcalBarrelDetId(iEta,iPhi,depthNr);
	cellList.push_back(getCell(cellDetId));
      }
      //now add in endcap part (which I like to think of barrel, depth 3 but is endcap depth 1)
      int cellDetId = DetIdTools::makeHcalEndcapDetId(iEta,iPhi,1);
      cellList.push_back(getCell(cellDetId));
      return cellList;
    }else if(iEtaAbs==29){ //last special case, the towers merge into one at depth 3
      //current strategy is to keep them seperate at depths 1,2 and take depth 3
      //so if it was in tower 29, we take 29,depth=1 29,depth=2 and 28 depth=3
      //first get first two depths of tower 29
      for(int depthNr=1;depthNr<=2;depthNr++){
	int cellDetId = DetIdTools::makeHcalEndcapDetId(iEta,iPhi,depthNr);
	cellList.push_back(getCell(cellDetId));
      }
      //now get the last depth of tower 28, first work out the side we are on
      int side= DetIdTools::zSideHcal(detId);
      int cellDetId = DetIdTools::makeHcalEndcapDetId(side*28,iPhi,3);
      cellList.push_back(getCell(cellDetId));
      return cellList;
    }else {
      std::cout <<" SHCaloGeom::getCellsInEtaPhiTower : Warning : iEta "<<iEta<<" is invalid "<<std::endl;
    }//end iEta test
  }else {//end of hcal test
    std::cout <<" SHCaloGeom::getCellsInEtaPhiTower : Warning : detId "<<std::hex<<detId<<std::dec<<" is not in ecal or hcal "<<std::endl;
  }
  return cellList; //only returned if there was an error condition, returns are done in the if statements (for no real reason)
}

float SHCaloGeom::phiDistToCellBoundary(const TVector3& centre)const
{
  const SHCaloCellGeom& cell = getClosestCell(centre);
  double dPhiMin = fabs(centre.Phi()-cell.frontEdges().minPhi);
  if(dPhiMin>MathFuncs::kPi) dPhiMin = MathFuncs::kPi*2-dPhiMin;
  else if(-dPhiMin<MathFuncs::kPi) dPhiMin = MathFuncs::kPi*2+dPhiMin;
  
  double dPhiMax = fabs(centre.Phi()-cell.frontEdges().maxPhi);
  if(dPhiMax>MathFuncs::kPi) dPhiMax = MathFuncs::kPi*2-dPhiMax;
  else if(-dPhiMax<-MathFuncs::kPi) dPhiMax = MathFuncs::kPi*2+dPhiMax;
  
  return dPhiMin < dPhiMax ? dPhiMin : dPhiMax;
}

float SHCaloGeom::etaDistToCellBoundary(const TVector3& centre)const
{
  const SHCaloCellGeom& cell = getClosestCell(centre);
  double dEtaMin = fabs(centre.Eta()-cell.frontEdges().minEta);
  double dEtaMax = fabs(centre.Eta()-cell.frontEdges().maxEta);
  return dEtaMin < dEtaMax ? dEtaMin : dEtaMax;
}
