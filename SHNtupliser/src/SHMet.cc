#include "SHarper/SHNtupliser/interface/SHMet.hh"

ClassImp(SHMet)

SHMet::SHMet():
  mEx_(-999.),mEy_(-999.),
  emEtEB_(-999.),emEtEE_(-999.),emEtHF_(-999.),
  hadEtHB_(-999.),hadEtHE_(-999.),hadEtHO_(-999.),hadEtHF_(-999.)  
{

}

// SHMet::SHMet(double mEx,double mEy,double emEt,double hadEt):
//   mEx_(mEx),mEy_(mEy),
//   emEt_(emEt),hadEt_(hadEt)
// {

// }

void SHMet::setMet(double mEx,double mEy)
{
  mEx_=mEx;
  mEy_=mEy;
}

void SHMet::setSumEmEt(double etEB,double etEE,double etHF)
{
  emEtEB_=etEB;
  emEtEE_=etEE;
  emEtHF_=etHF;
}

void SHMet::setSumHadEt(double etHB,double etHE,double etHO,double etHF)
{
  hadEtHB_=etHB;
  hadEtHE_=etHE;
  hadEtHO_=etHO;
  hadEtHF_=etHF;
}

void SHMet::clear()
{
  setMet(-999.,-999.);
  setSumEmEt(-999.,-999.,-999.);
  setSumHadEt(-999.,-999.,-999.,-999.);
}


