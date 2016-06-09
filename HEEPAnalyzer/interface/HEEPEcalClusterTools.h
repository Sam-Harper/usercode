#ifndef SHARPER_HEEPANALYZER_HEEPEcalClusterTools_h
#define SHARPER_HEEPANALYZER_HEEPEcalClusterTools_h

//a collection of functions in EcalClusterTools that have not yet been merged in the official code
//you should try not to use this if the function is already integrated in your release


#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
class DetId;
class CaloTopology;

namespace heep {
  class EcalClusterTools {
  public:
    static int nrSaturatedCrysIn5x5(const DetId& id,const EcalRecHitCollection* recHits,const CaloTopology *topology);
    
  };
}
#endif
