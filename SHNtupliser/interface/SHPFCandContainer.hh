#ifndef SHPFCANDCONTAINER_HH
#define SHPFCANDCONTAINER_HH

#include "SHarper/SHNtupliser/interface/SHPFCandidate.hh"
#include "TObject.h"
#include <algorithm>
#include <iostream>
class SHPFCandContainer {

private:
  std::vector<SHPFCandidate> chargedHad_;
  std::vector<SHPFCandidate> neutralHad_;
  std::vector<SHPFCandidate> photon_;

public:
  SHPFCandContainer(){}
  virtual ~SHPFCandContainer(){}

  SHPFCandidate& addChargedHad(float pt,float eta,float phi,float mass,
			       float mvaNothingGamma,int scSeedCrysId,
			       float hadNrgyRaw=0.,float hadNrgyMap=0.,float hadNrgyMapRaw=0.,
			       float hadClusterNrgy=0.){
    chargedHad_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId,hadNrgyRaw,hadNrgyMap,hadNrgyMapRaw,hadClusterNrgy));
    return chargedHad_.back();}
  SHPFCandidate& addNeutralHad(float pt,float eta,float phi,float mass,
			       float mvaNothingGamma,int scSeedCrysId,
			       float hadNrgyRaw=0,float hadNrgyMap=0.,float hadNrgyMapRaw=0.,
			       float hadClusterNrgy=0.){
    neutralHad_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId,hadNrgyRaw,hadNrgyMap,hadNrgyMapRaw,hadClusterNrgy));
    return neutralHad_.back();}
  SHPFCandidate& addPhoton(float pt,float eta,float phi,float mass,
			   float mvaNothingGamma,int scSeedCrysId,
			   float hadNrgyRaw=0,float hadNrgyMap=0.,float hadNrgyMapRaw=0.,
			   float hadClusterNrgy=0.){
    photon_.push_back(SHPFCandidate(pt,eta,phi,mass,mvaNothingGamma,scSeedCrysId,hadNrgyRaw,hadNrgyMap,hadNrgyMapRaw,hadClusterNrgy));
    return photon_.back();}
  
  SHPFCandidate& addPhoton(const SHPFCandidate& cand){photon_.push_back(cand);return photon_.back();}

  template<class T> void copy(const SHPFCandContainer& rhs,T cutFunc){
    clear();
    std::copy_if(rhs.photon_.begin(),rhs.photon_.end(),std::back_inserter(photon_),cutFunc);
    std::copy_if(rhs.neutralHad_.begin(),rhs.neutralHad_.end(),std::back_inserter(neutralHad_),cutFunc); 
    std::copy_if(rhs.chargedHad_.begin(),rhs.chargedHad_.end(),std::back_inserter(chargedHad_),cutFunc); 
  }
  void clear();

  size_t nrPhos()const{return photon_.size();}
  size_t nrChargedHads()const{return chargedHad_.size();}
  size_t nrNeutralHads()const{return neutralHad_.size();}
  
  const SHPFCandidate* photon(size_t nr)const{return &photon_[nr];}
  const SHPFCandidate* chargedHad(size_t nr)const{return &chargedHad_[nr];}
  const SHPFCandidate* neutralHad(size_t nr)const{return &neutralHad_[nr];}

  const std::vector<SHPFCandidate>& photons()const{return photon_;}
  const std::vector<SHPFCandidate>& chargedHad()const{return chargedHad_;}
  const std::vector<SHPFCandidate>& neutralHad()const{return neutralHad_;}


  ClassDef(SHPFCandContainer,1)


};


#endif
