#ifndef SHARPER_SHNTUPLISER_SHGEOMFILLER
#define SHARPER_SHNTUPLISER_SHGEOMFILLER

//class: SHGeomFiller
//Description: a class which fills my geometry classes
//
//Implimentation: 
// 
//Author: Sam Harper

#include "SHarper/SHNtupliser/interface/SHCaloCellGeom.hh"

class CaloGeometry;
class CaloTowerConstituentsMap;
class SHCaloGeom;
class DetId;
class TVector3;

namespace edm{
  class EventSetup;
}

class SHGeomFiller{
  
private:
  const CaloGeometry* calGeometry_;
  const CaloTowerConstituentsMap* calTowersConstits_;
  
public:
  SHGeomFiller(const edm::EventSetup& setup);
  ~SHGeomFiller(){}

  //the main user functions
  void fillEcalGeom(SHCaloGeom& ecalGeom);
  void fillHcalGeom(SHCaloGeom& hcalGeom);

  //the functions which do the actual work
  void fillEcalGeomBarrel(SHCaloGeom& ecalGeom);
  void fillEcalGeomEndcap(SHCaloGeom& ecalGeom);
  void fillHcalGeomBarrel(SHCaloGeom& hcalGeom);
  void fillHcalGeomEndcap(SHCaloGeom& hcalGeom);
  void getCellPosition(const DetId &detId,TVector3 & pos);
  void getCellEdges(const DetId& detId,SHCaloCellGeom::CellEdges& frontEdges,SHCaloCellGeom::CellEdges& rearEdges);
  void getCellCorners(const DetId& detId,std::vector<TVector3>& cornerVec);
};


#endif
