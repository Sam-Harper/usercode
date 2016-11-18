#include "SHarper/SHNtupliser/interface/SHJet.hh"

ClassImp(SHJet)

SHJet::SHJet():
  pt_(0.),eta_(0.),phi_(0.),nrgy_(0.),
  invNrgyCorr_(0.),
  neutralHadNrgy_(0.),
  neutralEmNrgy_(0.),
  chargedHadNrgy_(0.),
  chargedEmNrgy_(0.),
  muonNrgy_(0.),
  chargedMultiplicity_(0),
  neutralMultiplicity_(0),
  combinedSecVtxBTag_(0.),
  combinedMVABTag_(0.),
  partonFlavour_(0),
  hadronFlavour_(0)
{

}

 
bool SHJet::operator==(const SHJet& rhs)const
{
  auto match=[](const float& lhs,const float& rhs){return std::abs(lhs-rhs)<0.001;};
  return match(pt_,rhs.pt_) &&
    match(eta_,rhs.eta_) &&
    match(phi_,rhs.phi_);
}
