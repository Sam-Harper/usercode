#include "SHarper/SHNtupliser/interface/SHPreShowerCluster.hh"

ClassImp(SHPreShowerCluster)
SHPreShowerCluster::SHPreShowerCluster():
  nrgy_(0.),
  pos_(0.,0.,0.), 
  eta_(0.),
  plane_(-1.)
{
 
}

std::ostream& SHPreShowerCluster::print(std::ostream& output)const
{
  output <<" nrgy "<<nrgy()<<" et "<<et()<<" eta "<<eta()<<" phi "<<phi()<<" nrHits "<<nrHits()<<" plane "<<plane();
  return output;
}

std::ostream &operator <<(std::ostream& output,const SHPreShowerCluster& clus)
{
  return clus.print(output);
}
