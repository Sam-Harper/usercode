#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"

#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "TGraph.h"

ClassImp(SHL1Cluster)

SHL1Cluster::SHL1Cluster():
  etTS_(-999),
  emEtOverTotEt_(0),
  iEta_(0),iPhi_(0),
  isolEmEtTS_(0),isolHadEtTS_(0),
  trigScale_(0),
  isIsolated_(false),
  p4_(),
  hademTS_(0),
  clusterFlags_(0),
  etSeedTS_(0),
  quality_(0)
{

}

int SHL1Cluster::iEtaMax()const
{
  return iEta();
}

int SHL1Cluster::iPhiMax()const
{
  return iPhi();
}


std::ostream& SHL1Cluster::print(std::ostream& output)const
{
  output <<" etTS "<<etTS()<<" iEta "<<iEta()<<" iPhi "<<iPhi()<<" emEtOverTotEt "<<emEtOverTotEt()<<" isolEmEtTS "<<isolEmEtTS()<<" isolHadEtTS "<<isolHadEtTS()<<" isIsolated "<<isIsolated()<<" et "<<p4().Et()<<" nrgy "<<p4().E()<<" eta "<<p4().Eta()<<" phi "<<p4().Phi();
  return output;


}

std::ostream& operator <<(std::ostream& output,const SHL1Cluster &clus)
{
  return clus.print(output);
}


float SHL1Cluster::eta()const
{
  if(p4_.Et()>0.1) return p4_.Eta();
  else {

    const float towerEtas[29] = {0,.087,0.174,0.261,0.348,0.435,0.522,0.609,0.696,0.783,0.879,0.957,1.044,1.131,1.218,1.305,1.392,1.479,1.566,1.653,1.740,1.830,1.930,2.0432,2.1732,2.322,2.5,2.650,3.000};
  
    float eta = (towerEtas[abs(iEta())]+towerEtas[abs(iEta())-1])/2;
    float sign = iEta()>0 ? 1 : -1;
    return eta*sign;
  }
}

float SHL1Cluster::phi()const
{
  if(p4_.Et()>0.1) return p4_.Phi();
  else return (iPhi()-0.5)*5/180*3.14;

}

const TLorentzVector& SHL1Cluster::p4()const
{
  if(p4_.Et()>0.1) return p4_;
  else{ //little hack for when this wasnt filled, only stage 2 valid
    p4Temp_.SetPtEtaPhiM(etTS()*trigScale(),eta(),phi(),0);
    return p4Temp_;
  }
}
