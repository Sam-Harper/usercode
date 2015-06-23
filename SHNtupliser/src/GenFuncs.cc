
#include "SHarper/SHNtupliser/interface/GenFuncs.h"
#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"
#include "SHarper/SHNtupliser/interface/SHGenInfo.hh"

#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"


SHMCParticle GenFuncs::makeMCParticle(const reco::GenParticle* genPart,const std::vector<reco::GenParticle>& particles)
{
  int index = std::distance(&particles[0],genPart);
  int pid = genPart->pdgId();
  int status = genPart->status(); //note this is not accessible in CMSSW_1_3_5
  int nrMo = genPart->numberOfMothers();
  int nrDa = genPart->numberOfDaughters();
  TLorentzVector p4(genPart->px(),genPart->py(),genPart->pz(),genPart->energy());
  TVector3 pos(genPart->vx(),genPart->vy(),genPart->vz());
    
   
  //now sort out the mother and daughter links
  int jmo1 = -1;
  int jmo2 = -1;
  int jda1 = -1;
  int jda2 = -1;
  auto daughter = static_cast<const reco::GenParticle*>(genPart->daughter(0));
  auto mother = static_cast<const reco::GenParticle*>(genPart->mother(0));
  if(mother){
    jmo1 = std::distance(&particles[0],mother);
    jmo2 = jmo1 + nrMo-1;
  }
  if(daughter){
    jda1 = std::distance(&particles[0],daughter);
    jda2 = jda1 + nrDa-1;
  }
  
  return SHMCParticle(index,status,pid,jmo1,jmo2,nrMo,jda1,jda2,nrDa,p4,pos);
  
}

void GenFuncs::fillGenInfo(const heep::Event& heepEvt,SHGenInfo& genInfo)
{
  if(!heepEvt.hasGenParticles()) return;
  const std::vector<reco::GenParticle>& particles = heepEvt.genParticles();


  std::vector<const reco::Candidate*> partsToStore;
  for(const auto& mcPart : particles){
    if(mcPart.pdgId()==2212 && mcPart.status()==4){ //lets save the protons, why not!
      partsToStore.push_back(&mcPart);
    }else if(mcPart.status()>=20 && mcPart.status()<=29){ //just gets the initial hard interaction in pythia 8 
      //now for these we get the all the daughters of anything thats not a gluon
      partsToStore.push_back(&mcPart);
      if(mcPart.pdgId()!=21 && !(std::abs(mcPart.pdgId())>=1 && std::abs(mcPart.pdgId())<=5)) getAllDaughters(&mcPart,partsToStore);
    }
  }
  std::sort(partsToStore.begin(),partsToStore.end());
  const reco::Candidate* lastGenPart=nullptr;
  for(auto part : partsToStore){
    if(part==lastGenPart) continue;
    lastGenPart=part;
    genInfo.addMCParticle(makeMCParticle(static_cast<const reco::GenParticle*>(part),particles));
  }
}

void GenFuncs::getAllDaughters(const reco::Candidate* part,
			       std::vector<const reco::Candidate*>& daughters)
{
  for(size_t daNr=0;daNr<part->numberOfDaughters();daNr++){
    const reco::Candidate* daughter = part->daughter(daNr);
    daughters.push_back(daughter);
    getAllDaughters(daughter,daughters);
  }
}
