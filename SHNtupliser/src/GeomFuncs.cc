#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"

#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "TFile.h"



SHCaloGeom* GeomFuncs::ecalGeom_(NULL);
SHCaloGeom* GeomFuncs::hcalGeom_(NULL);
std::vector<float> GeomFuncs::ebGapNegEtas_;
std::vector<float> GeomFuncs::ebGapPosEtas_;

const SHCaloGeom& GeomFuncs::getEcalGeom()
{
  if(ecalGeom_!=NULL) return *ecalGeom_;
  else{
    std::cout <<"GeomFuncs::getEcalGeom : Error ecalGeom_ not loaded"<<std::endl;
    return *ecalGeom_;
  }
}


const SHCaloGeom& GeomFuncs::getHcalGeom()
{
  if(hcalGeom_!=NULL) return *hcalGeom_;
  else{
    std::cout <<"GeomFuncs::getHcalGeom : Error hcalGeom_ not loaded"<<std::endl;
    return *hcalGeom_;
  }
} 

const SHCaloCellGeom& GeomFuncs::returnCellBadDetId_(int detId)
{
  std::cout <<"GeomFuncs::returnCellBadGeom : Error det id "<<std::hex<<detId<<std::dec<<" not ecal or hcal"<<std::endl;
  SHCaloCellGeom* nullP = NULL;
  return *nullP;
}

const SHCaloCellGeom& GeomFuncs::returnCellNoGeom_()
{
  std::cout <<"GeomFuncs::returnCellBadGeom : Error geom not loaded "<<std::endl;
  SHCaloCellGeom* nullP = NULL;
  return *nullP;
}

// const SHCaloCellGeom& GeomFuncs::getCell(int detId)
// {
//   if(DetIdTools::isEcal(detId)) return ecalGeom_->getCell(detId);
//   else if(DetIdTools::isHcal(detId)) return hcalGeom_->getCell(detId);
//   else{
//     std::cout <<"GeomFuncs::getCell : Error det id "<<std::hex<<detId<<std::dec<<" not ecal or hcal"<<std::endl;
//     SHCaloCellGeom* nullP = NULL;
//     return *nullP;
//   }
// }

// void GeomFuncs::getCellEtaPhi(int detId,double& eta,double& phi)
// {
//   const SHCaloCellGeom& cell = DetIdTools::isEcal(detId) ? ecalGeom_->getCell(detId) : hcalGeom_->getCell(detId);
//   eta= cell.eta();
//   phi= cell.phi();
// }


// const SHCaloCellGeom& GeomFuncs::getEcalCell(int detId)
// {
//   if(ecalGeom_!=NULL){
//     return ecalGeom_->getCell(detId);
//   }else{
//     std::cout <<"GeomFuncs::getEcalCell : Error ecalGeom_ not loaded"<<std::endl;
//     SHCaloCellGeom* nullPointer=NULL;
//     return *nullPointer;
//   }
// }


// const SHCaloCellGeom & GeomFuncs::getHcalCell(int detId)
// {
//   if(hcalGeom_!=NULL){
//     return hcalGeom_->getCell(detId);
//   }else{
//     std::cout <<"GeomFuncs::getHcalCell : Error hcalGeom_ not loaded"<<std::endl;
//     SHCaloCellGeom* nullPointer=NULL;
//     return *nullPointer;
//   }
// }


void GeomFuncs::loadCaloGeom(const char* filename)
{
  TFile* file = TFile::Open(filename,"READ");
  ecalGeom_ = (SHCaloGeom*) file->Get("ecalGeom");
  hcalGeom_ = (SHCaloGeom*) file->Get("hcalGeom"); 
  setEBGapEtas_();

  file->Close();
  delete file;
}


void GeomFuncs::loadCaloGeom(const SHCaloGeom& ecalGeom,const SHCaloGeom& hcalGeom)
{
  if(ecalGeom_) delete ecalGeom_;
  if(hcalGeom_) delete hcalGeom_;
  ecalGeom_ = new SHCaloGeom(ecalGeom);
  hcalGeom_ = new SHCaloGeom(hcalGeom);
}

void GeomFuncs::deleteCaloGeom()
{
  delete ecalGeom_;
  delete hcalGeom_;
  ecalGeom_=NULL;
  hcalGeom_=NULL;
  ebGapPosEtas_.clear();
  ebGapNegEtas_.clear();
}

void GeomFuncs::setEBGapEtas_()
{
  ebGapPosEtas_.clear();
  ebGapPosEtas_.resize(5);
  ebGapNegEtas_.clear();
  ebGapNegEtas_.resize(5);

  //have to do this with HCAL cells as I forgot to fill ecal cell edges
  
 //  ebGapPosEtas_[0] = getCell(DetIdTools::makeEcalBarrelId(1,1)).frontEdges().minEta;
//   ebGapPosEtas_[1] = getCell(DetIdTools::makeEcalBarrelId(26,1)).frontEdges().minEta;
//   ebGapPosEtas_[2] = getCell(DetIdTools::makeEcalBarrelId(46,1)).frontEdges().minEta;
//   ebGapPosEtas_[3] = getCell(DetIdTools::makeEcalBarrelId(66,1)).frontEdges().minEta;
//   ebGapPosEtas_[4] = getCell(DetIdTools::makeEcalBarrelId(85,1)).frontEdges().maxEta;

//   ebGapNegEtas_[0] = getCell(DetIdTools::makeEcalBarrelId(-1,1)).frontEdges().maxEta;
//   ebGapNegEtas_[1] = getCell(DetIdTools::makeEcalBarrelId(-26,1)).frontEdges().maxEta;
//   ebGapNegEtas_[2] = getCell(DetIdTools::makeEcalBarrelId(-46,1)).frontEdges().maxEta;
//   ebGapNegEtas_[3] = getCell(DetIdTools::makeEcalBarrelId(-66,1)).frontEdges().maxEta;
//   ebGapNegEtas_[4] = getCell(DetIdTools::makeEcalBarrelId(-85,1)).frontEdges().minEta;

  ebGapPosEtas_[0] = getCell(DetIdTools::makeHcalBarrelDetId(1,1,1)).frontEdges().minEta;
  ebGapPosEtas_[1] = getCell(DetIdTools::makeHcalBarrelDetId(6,1,1)).frontEdges().minEta;
  ebGapPosEtas_[2] = getCell(DetIdTools::makeHcalBarrelDetId(10,1,1)).frontEdges().minEta;
  ebGapPosEtas_[3] = getCell(DetIdTools::makeHcalBarrelDetId(14,1,1)).frontEdges().minEta;
  ebGapPosEtas_[4] = getCell(DetIdTools::makeHcalEndcapDetId(17,1,1)).frontEdges().maxEta;

  ebGapNegEtas_[0] = getCell(DetIdTools::makeHcalBarrelDetId(-1,1,1)).frontEdges().maxEta;
  ebGapNegEtas_[1] = getCell(DetIdTools::makeHcalBarrelDetId(-6,1,1)).frontEdges().maxEta;
  ebGapNegEtas_[2] = getCell(DetIdTools::makeHcalBarrelDetId(-10,1,1)).frontEdges().maxEta;
  ebGapNegEtas_[3] = getCell(DetIdTools::makeHcalBarrelDetId(-14,1,1)).frontEdges().maxEta;
  ebGapNegEtas_[4] = getCell(DetIdTools::makeHcalEndcapDetId(-17,1,1)).frontEdges().minEta;
}
