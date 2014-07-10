#include "SHarper/SHNtupliser/interface/SHPFCluster.hh"

ClassImp(SHPFCluster)

SHPFCluster::SHPFCluster():
  superClusId_(0),
  caloId_(0),
  flags_(0),
  seedId_(0),
  nrgy_(0.)
{
  pos_.SetPtEtaPhi(0.0001,-9999,0);
}
