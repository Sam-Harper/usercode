#ifndef GEOMFUNCS
#define GEOMFUNCS

//basically a global interface to the CaloGeom, currently migrating from EcalCrysPos and HcalCellPos to SHCaloGeom

#include "SHarper/SHNtupliser/interface/SHCaloGeom.hh"
#include "SHarper/SHNtupliser/interface/SHCaloCellGeom.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "TVector3.h"

#include <iostream>

//a static class
class GeomFuncs{

private:
  static SHCaloGeom* ecalGeom_;
  static SHCaloGeom* hcalGeom_;
  //the postion of the ecal barrel gaps (both vectors contain gap at zero)
  //in theory they should be identical except for a sign change
  //in practice I'm not sure this is 100% guaranteed...
  static std::vector<float> ebGapPosEtas_;
  static std::vector<float> ebGapNegEtas_;
private:
  GeomFuncs(){}
  ~GeomFuncs(){}
public:
  static inline const SHCaloCellGeom& getCell(int detId);
  static inline const SHCaloCellGeom& getEcalCell(int detId);
  static inline const SHCaloCellGeom& getHcalCell(int detId);
  
  static const TVector3& getCellPos(int detId){return getCell(detId).pos();}
  static inline void getCellEtaPhi(int detId,double& eta,double& phi);
  
  static const SHCaloGeom& getHcalGeom();
  static const SHCaloGeom& getEcalGeom(); 

  static const std::vector<float>& ebGapPosEtas(){return ebGapPosEtas_;}
  static const std::vector<float>& ebGapNegEtas(){return ebGapNegEtas_;}


  static void loadCaloGeom(const char* filename);
  static void loadCaloGeom(const SHCaloGeom& ecalGeom,const SHCaloGeom& hcalGeom); //makes copies of the geom
  static void deleteCaloGeom();

private: 
  //the idea is that functions in this class are accessed *alot* so I want to seperte them up into parts to inline and parts to not
  //not that the idea actually worked but hey
  static bool isGeomLoaded_(){return hcalGeom_!=NULL && ecalGeom_!=NULL;}
  static const SHCaloCellGeom& returnCellNoGeom_();//returns a null reference (yes I know I'm evil) if the geom isnt loaded and also prints a message to the effect)
  static bool validDetId_(int detId){return DetIdTools::isEcal(detId) || DetIdTools::isHcal(detId);}
  static const SHCaloCellGeom& returnCellBadDetId_(int detId);//returns a null reference (yes I know I'm evil) if the det id is bad isnt loaded and also prints a message to the effect)
  static void setEBGapEtas_();
};

//this function is called *alot* hence inlineing
void GeomFuncs::getCellEtaPhi(int detId,double& eta,double& phi)
{
  SHCaloGeom* geom = DetIdTools::isEcal(detId) ? ecalGeom_ : hcalGeom_;
  if(geom!=NULL){
    const SHCaloCellGeom& cell = geom->getCell(detId);
    eta= cell.eta();
    phi= cell.phi();
  }else std::cout <<"GeomFuncs::getCellEtaPhi : Error geometry not loaded"<<std::endl;
}

const SHCaloCellGeom& GeomFuncs::getEcalCell(int detId)
{ 
  if(!isGeomLoaded_()) return returnCellNoGeom_();
  else if(!validDetId_(detId)) return returnCellBadDetId_(detId); 
  else return ecalGeom_->getCell(detId);
}


const SHCaloCellGeom & GeomFuncs::getHcalCell(int detId)
{
  if(!isGeomLoaded_()) return returnCellNoGeom_();
  else if(!validDetId_(detId)) return returnCellBadDetId_(detId); 
  else {
    return hcalGeom_->getCell(detId);
  }
}


const SHCaloCellGeom& GeomFuncs::getCell(int detId)
{
  if(!isGeomLoaded_()) return returnCellNoGeom_();
  else if(!validDetId_(detId)) return returnCellBadDetId_(detId); 
  else {
    if(DetIdTools::isEcal(detId)) return ecalGeom_->getCell(detId);
    else return hcalGeom_->getCell(detId);
  }
}

#endif
