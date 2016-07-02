#ifndef SHARPER_SHNTUPLISER_GENFUNCS_H
#define SHARPER_SHNTUPLISER_GENFUNCS_H

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

class SHMCParticle;
class SHGenInfo;
namespace heep{
  class Event;

}

class GenFuncs {
private:
  
  static constexpr int kNrPartThresToStoreAll=10; //if the number of gen particles is <= this value, we store all of them
  
  GenFuncs()=delete;
public:
  static SHMCParticle makeMCParticle(const reco::GenParticle* genPart,const std::vector<reco::GenParticle>& particles);

  static void fillGenInfo(const heep::Event& heepEvt,SHGenInfo& genInfo,bool addMCParts);

  static void getAllDaughters(const reco::Candidate* part,
			      std::vector<const reco::Candidate*>& daughters);
  static std::pair<int,int> findDaughters(int partNr,const std::vector<std::pair<int,int> >& mothers);
  static void fillPDFInfo(const heep::Event& heepEvt,SHGenInfo& genInfo);
  static void fillMCParticles(const heep::Event& heepEvt,SHGenInfo& genInfo);
  static void fillLHEParticles(const heep::Event& heepEvt,SHGenInfo& genInfo);
};

#endif
