#ifndef SHPFCANDCONTAINER_HH
#define SHPFCANDCONTAINER_HH

#include "SHarper/SHNtupliser/interface/SHPFCandidate.hh"
#include "TObject.h"

class SHPFCandContainer {

private:
  std::vector<SHPFCandidate> chargedHad_;
  std::vector<SHPFCandidate> neutralHad_;
  std::vector<SHPFCandidate> photon_;

public:
  SHPFCandContainer(){}
  virtual ~SHPFCandContainer(){}

  SHPFCandidate& addChargedHad(float pt,float eta,float phi,float mass,float mvaNothingGamma,int scSeedCrysId){chargedHad_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId));return chargedHad_.back();}
  SHPFCandidate& addNeutralHad(float pt,float eta,float phi,float mass,float mvaNothingGamma,int scSeedCrysId){neutralHad_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId));return neutralHad_.back();}
  SHPFCandidate& addPhoton(float pt,float eta,float phi,float mass,float mvaNothingGamma,int scSeedCrysId){photon_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId));return photon_.back();}

  void clear();

  size_t nrPhos()const{return photon_.size();}
  size_t nrChargedHads()const{return chargedHad_.size();}
  size_t nrNeutralHads()const{return neutralHad_.size();}
  
  const SHPFCandidate* photon(size_t nr)const{return &photon_[nr];}
  const SHPFCandidate* chargedHad(size_t nr)const{return &chargedHad_[nr];}
  const SHPFCandidate* neutralHad(size_t nr)const{return &neutralHad_[nr];}

  ClassDef(SHPFCandContainer,1)


};


#endif
