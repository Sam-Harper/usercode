#include "SHarper/SHNtupliser/interface/SHJet.hh"

ClassImp(SHJet)

SHJet::SHJet():
  p4_(0.,0.,0.,0.),
  hadFrac_(0.),
  caloArea_(0.),
  maxNrgyInEmTowers_(0.),
  maxNrgyInHadTowers_(0.),
  hadNrgyInH0_(0.),
  hadNrgyInHB_(0.),
  hadNrgyInHF_(0.),
  hadNrgyInHE_(0.),
  emNrgyInEB_(0.),
  emNrgyInEE_(0.),
  emNrgyInHF_(0.),
  jetNrgyScale_(1.),  
  trkCountHighEffBTag_(-10.),
  trkCountHighPureBTag_(-10.),
  jetBProbBTag_(-1.),
  jetProbBTag_(-1.),
  partonFlavour_(0)
{

}

SHJet::SHJet(const SHJet& rhs):
  p4_(rhs.p4_),
  hadFrac_(rhs.hadFrac_),
  caloArea_(rhs.caloArea_),
  maxNrgyInEmTowers_(rhs.maxNrgyInEmTowers_),
  maxNrgyInHadTowers_(rhs.maxNrgyInHadTowers_),
  hadNrgyInH0_(rhs.hadNrgyInH0_),
  hadNrgyInHB_(rhs.hadNrgyInHB_),
  hadNrgyInHF_(rhs.hadNrgyInHF_),
  hadNrgyInHE_(rhs.hadNrgyInHE_),
  emNrgyInEB_(rhs.emNrgyInEB_),
  emNrgyInEE_(rhs.emNrgyInEE_),
  emNrgyInHF_(rhs.emNrgyInHF_),
  jetNrgyScale_(rhs.jetNrgyScale_),
  trkCountHighEffBTag_(rhs.trkCountHighEffBTag_),
  trkCountHighPureBTag_(rhs.trkCountHighPureBTag_),
  jetBProbBTag_(rhs.jetBProbBTag_),
  jetProbBTag_(rhs.jetBProbBTag_),
  partonFlavour_(rhs.partonFlavour_)
{

}

SHJet& SHJet::operator=(const SHJet &rhs)
{
  if(&rhs!=this){ //checking for self assignment, not that it actually matters in this case
    p4_ = rhs.p4_;
    hadFrac_ = rhs.hadFrac_;
    caloArea_ = rhs.caloArea_;
    maxNrgyInEmTowers_ = rhs.maxNrgyInEmTowers_;
    maxNrgyInHadTowers_ = rhs.maxNrgyInHadTowers_;
    hadNrgyInH0_ = rhs.hadNrgyInH0_;
    hadNrgyInHB_ = rhs.hadNrgyInHB_;
    hadNrgyInHF_ = rhs.hadNrgyInHF_;
    hadNrgyInHE_ = rhs.hadNrgyInHE_;
    emNrgyInEB_ = rhs.emNrgyInEB_;
    emNrgyInEE_ = rhs.emNrgyInEE_;
    emNrgyInHF_ = rhs.emNrgyInHF_;
    jetNrgyScale_ = rhs.jetNrgyScale_; 
    trkCountHighEffBTag_ = rhs.trkCountHighEffBTag_;
    trkCountHighPureBTag_ = rhs.trkCountHighPureBTag_;
    jetBProbBTag_ = rhs.jetBProbBTag_;
    jetProbBTag_ = rhs.jetBProbBTag_;
    partonFlavour_ = rhs.partonFlavour_;
  } 
  return *this;
}


bool SHJet::operator==(const SHJet& rhs)const
{
  if(p4_ == rhs.p4_) return true;
  else return false;
}
