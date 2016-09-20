#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"

#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"

#include <algorithm>

SHBasicCluster::SHBasicCluster(const reco::BasicCluster& clus):
  totNrgy_(clus.energy()),
  nrCrys_(clus.hitsAndFractions().size()),
  eta_(clus.position().eta()),
  phi_(clus.position().phi()), 
  seedId_(clus.seed())
{
  
}
