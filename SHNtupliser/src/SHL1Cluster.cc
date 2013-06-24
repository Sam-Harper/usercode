#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"

ClassImp(SHL1Cluster)

SHL1Cluster::SHL1Cluster():
  etTS_(-999),
  emEtOverTotEt_(0),
  iEta_(0),iPhi_(0),
  isolEmEtTS_(0),isolHadEtTS_(0),
  trigScale_(0),
  fgVeto_(false),
  isIsolated_(false),
  p4_(),
  leadTower_(-1),
  constituents_(0)
{

}


std::ostream& SHL1Cluster::print(std::ostream& output)const
{
  output <<" etTS "<<etTS()<<" iEta "<<iEta()<<" iPhi "<<iPhi()<<" emEtOverTotEt "<<emEtOverTotEt()<<" isolEmEtTS "<<isolEmEtTS()<<" isolHadEtTS "<<isolHadEtTS()<<" fg "<<fgVeto()<<" isIsolated "<<isIsolated()<<" et "<<p4().Et()<<" nrgy "<<p4().E()<<" eta "<<p4().Eta()<<" phi "<<p4().Phi();
  return output;


}

std::ostream& operator <<(std::ostream& output,const SHL1Cluster &clus)
{
  return clus.print(output);
}
