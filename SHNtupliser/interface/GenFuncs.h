#ifndef SHARPER_SHNTUPLISER_GENFUNCS_H
#define SHARPER_SHNTUPLISER_GENFUNCS_H

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

class SHMCParticle;
class SHGenInfo;
namespace heep{
  class Event;

}

class GenFuncs {
  
  GenFuncs()=delete;
public:
  static SHMCParticle makeMCParticle(const reco::GenParticle* genPart,const std::vector<reco::GenParticle>& particles);

  static void fillGenInfo(const heep::Event& heepEvt,SHGenInfo& genInfo);

  static void getAllDaughters(const reco::Candidate* part,
			      std::vector<const reco::Candidate*>& daughters);
};

#endif
