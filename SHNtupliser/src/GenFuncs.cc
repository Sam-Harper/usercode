
#include "SHarper/SHNtupliser/interface/GenFuncs.h"
#include "SHarper/SHNtupliser/interface/SHMCParticle.hh"
#include "SHarper/SHNtupliser/interface/SHGenInfo.hh"

#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"

#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

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

  fillLHEParticles(heepEvt,genInfo);
  fillMCParticles(heepEvt,genInfo);
  fillPDFInfo(heepEvt,genInfo);
  if(heepEvt.handles().genEvtInfo.isValid()) genInfo.setWeight(heepEvt.handles().genEvtInfo->weight());
  else genInfo.setWeight(1);
}

void GenFuncs::fillLHEParticles(const heep::Event& heepEvt,SHGenInfo& genInfo)
{
  edm::Handle<LHEEventProduct> lheEventHandle = heepEvt.handles().lheEvent;
  if(!lheEventHandle.isValid()) return;

  const lhef::HEPEUP& hepeup = lheEventHandle->hepeup();
  int nrParts  = hepeup.NUP;
  TVector3 pos(0,0,0); //all at zero 
  for(int partNr=0;partNr<nrParts;partNr++){
    
    int mother1=hepeup.MOTHUP[partNr].first;
    int mother2=hepeup.MOTHUP[partNr].second;
    int nrMothers=mother2-mother1;
    
    std::pair<int,int> daughts=findDaughters(partNr,hepeup.MOTHUP);

    int daught1=daughts.first;
    int daught2=daughts.second;
    int nrDaughts=daught2-daught1;

    TLorentzVector p4;
    p4.SetXYZT(hepeup.PUP[partNr][0],hepeup.PUP[partNr][1],hepeup.PUP[partNr][2],
	       hepeup.PUP[partNr][3]);

    SHMCParticle part(partNr,hepeup.ISTUP[partNr],hepeup.IDUP[partNr],
		      mother1,mother2,nrMothers,
		      daught1,daught2,nrDaughts,
		      p4,pos);
    genInfo.addLHEParticle(part);
  }
  
}

std::pair<int,int> GenFuncs::findDaughters(int partNr,const std::vector<std::pair<int,int> >& mothers)
{
  std::vector<size_t> daughters;

  for(size_t mothPartNr=0;mothPartNr<mothers.size();mothPartNr++){
    if(partNr>=mothers[mothPartNr].first && partNr<=mothers[mothPartNr].second){
      daughters.push_back(mothPartNr);
    }
  }
  std::sort(daughters.begin(),daughters.end());
  if(daughters.empty()) return std::pair<int,int>(-1,-1);
  else return std::pair<int,int>(daughters.front(),daughters.back());
}

void GenFuncs::fillPDFInfo(const heep::Event& heepEvt,SHGenInfo& genInfo)
{
  if(!heepEvt.handles().genEvtInfo.isValid()) return;
  
  const gen::PdfInfo* pdfInfo = heepEvt.handles().genEvtInfo->pdf();
  
  genInfo.addPDFInfo(SHPDFInfo(pdfInfo->id,pdfInfo->x,pdfInfo->xPDF,pdfInfo->scalePDF));

}

void GenFuncs::fillMCParticles(const heep::Event& heepEvt,SHGenInfo& genInfo)
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
