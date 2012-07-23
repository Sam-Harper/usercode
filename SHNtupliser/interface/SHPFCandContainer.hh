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
  void addChargedHad(float pt,float eta,float phi,float mass,float mvaNothingGamma,int scSeedCrysId){chargedHad_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId));}
  void addNeutralHad(float pt,float eta,float phi,float mass,float mvaNothingGamma,int scSeedCrysId){neutralHad_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId));}
  void addPhoton(float pt,float eta,float phi,float mass,float mvaNothingGamma,int scSeedCrysId){photon_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId));}

  void clear();

  ClassDef(SHPFCandContainer,1)


};


#endif
