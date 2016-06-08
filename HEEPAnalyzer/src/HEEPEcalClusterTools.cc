#include "SHarper/HEEPAnalyzer/interface/HEEPEcalClusterTools.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "RecoCaloTools/Navigation/interface/CaloNavigator.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"

int heep::EcalClusterTools::nrSaturatedCrysIn5x5(const DetId& id,const EcalRecHitCollection* recHits,const CaloTopology *topology)
{
  int nrSat=0;
  CaloNavigator<DetId> cursor = CaloNavigator<DetId>( id, topology->getSubdetectorTopology( id ) );
  
  for ( int eastNr = -2; eastNr <= 2; ++eastNr ) { //east is eta in barrel
    for ( int northNr = -2; northNr <= 2; ++northNr ) { //north is phi in barrel
      cursor.home();
      cursor.offsetBy( eastNr, northNr);
      DetId id = *cursor;
      auto recHitIt = recHits->find(id);
      if(recHitIt!=recHits->end() && 
	 recHitIt->checkFlag(EcalRecHit::kSaturated)){
	nrSat++;
      }
		
    }
  }
  return nrSat;
}
