#ifndef SHARPER_SHNTUPLISER_SHGEOMFILLER
#define SHARPER_SHNTUPLISER_SHGEOMFILLER

//class: SHGeomFiller
//Description: a class which fills my geometry classes
//
//Implimentation: 
// 
//Author: Sam Harper

#include "SHarper/SHNtupliser/interface/SHCaloCellGeom.hh"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"

#include "Geometry/CaloTopology/interface/CaloTowerConstituentsMap.h"
class SHCaloGeom;
class DetId;
class TVector3;

namespace edm{
  class EventSetup;
}

class SHGeomFiller{
  
private:
  const edm::ESGetToken<CaloGeometry,CaloGeometryRecord> calGeomToken_;
  const edm::ESGetToken<CaloTowerConstituentsMap,CaloGeometryRecord> calTowersConstitsToken_;
  edm::ESHandle<CaloGeometry> calGeometry_;
  edm::ESHandle<CaloTowerConstituentsMap> calTowersConstits_;
  
public:

  SHGeomFiller(edm::ConsumesCollector&& cc);
  ~SHGeomFiller()=default;

  void initRun(const edm::EventSetup& setup);
  
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
