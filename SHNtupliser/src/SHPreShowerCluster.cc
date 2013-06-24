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
  output <<" nrgy "<<nrgy()<<" et "<<et()<<" eta "<<eta()<<" phi "<<phi()<<" nrHits "<<nrHits()<<" plane "<<plane()<<std::endl;
  output <<"hits: "<<std::endl;
  for(int hitNr=0;hitNr<nrHits();hitNr++){
    int detId = getHitsByDetId()[hitNr];
    output <<" hit "<<hitNr<<" / "<<nrHits()<<" plane "<<DetIdTools::planeES(detId)<<" zside "<<DetIdTools::zSideES(detId)<<" sensor x "<<DetIdTools::sixES(detId)<<" sensor y "<<DetIdTools::siyES(detId)<<" strip nr "<<DetIdTools::stripES(detId)<<std::endl;
  }
  return output;
  
}

std::ostream &operator <<(std::ostream& output,const SHPreShowerCluster& clus)
{
  return clus.print(output);
}
