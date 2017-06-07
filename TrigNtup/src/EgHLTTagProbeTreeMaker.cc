#include "SHarper/TrigNtup/interface/EgHLTTagProbeTreeMaker.hh"

#include "SHarper/TrigNtup/interface/TrigFuncs.hh"
#include "SHarper/TrigNtup/interface/CaloTools.hh"
#include "SHarper/SHNtupliser/interface/MathFuncs.hh"


EgHLTTagProbeTreeMaker::EgHLTTagProbeTreeMaker(const char* treeName,const char* treeTitle):
  minEtCut_(-1)
{
  tree_= new TTree(treeName,treeTitle);
  data_.createBranches(tree_);
}

void EgHLTTagProbeTreeMaker::fill(const SHEvent* event)
{
  auto trigSum = event->getTrigSum();
  auto tags = trigSum.getTrigObjs(tagFilter_);
  auto probes = trigSum.getTrigObjs(probeFilter_);
  
  data_.evt.fill(event);
  data_.weight = event->weight();
  data_.genWeight = event->getGenInfo().weight();
  data_.genPU.fill(*event);
  data_.evtTrigs = TrigBitsDef::getTrigCode(*event);
  for(const auto& tag: tags){
    for(const auto& probe : probes){
      if(tag!=probe){
	fill_(*tag,*probe,event);
      }
    }
  }
}




void EgHLTTagProbeTreeMaker::fill_(const SHTrigObj& tag,const SHTrigObj& probe,const SHEvent* event)
{

  
  const SHMCParticle* tagTruth = matchTruth_(tag.eta(),tag.phi(),0.1,*event);
  const SHElectron* tagReco = matchRecoEle_(tag.eta(),tag.phi(),0.1,*event);
  
  const SHMCParticle* probeTruth = matchTruth_(probe.eta(),probe.phi(),0.1,*event);
  const SHElectron* probeReco = matchRecoEle_(probe.eta(),probe.phi(),0.1,*event);
  

  data_.tagTrigs = TrigBitsDef::getTrigCode(*event,tag.eta(),tag.phi(),tag.eta(),tag.phi());
  data_.probeTrigs = TrigBitsDef::getTrigCode(*event,probe.eta(),probe.phi(),probe.eta(),probe.phi());
  data_.tagHLT.fill(tag);
  data_.probeHLT.fill(probe);

  if(tagTruth){
    data_.tagMC.fill(tagTruth->p4(),tagTruth->detEta());
    data_.tagTruthZ = tagTruth->pos().Z();
    data_.tagTruthRegion = CaloTools::region(tagTruth->detEta());
    data_.tagPID = tagTruth->pid();
    data_.tagTruthDR = MathFuncs::calDeltaR(tagTruth->p4(),tag.p4());
  }else{
    data_.tagMC.clear();
    data_.tagTruthZ = -999;
    data_.tagTruthRegion = -999;
    data_.tagPID = 0;
    data_.tagTruthDR = 999;
  }
  if(probeTruth){
    data_.probeMC.fill(probeTruth->p4(),probeTruth->detEta());
    data_.probeTruthZ = probeTruth->pos().Z();
    data_.probeTruthRegion = CaloTools::region(probeTruth->detEta());
    data_.probePID = probeTruth->pid();
    data_.probeTruthDR = MathFuncs::calDeltaR(probeTruth->p4(),probe.p4());
  }else{
    data_.probeMC.clear();
    data_.probeTruthZ = -999;
    data_.probeTruthRegion = -999;
    data_.probePID = 0;
    data_.probeTruthDR = 999;
  }
  
  
  if(tagReco){
    data_.tagReco.fill(*tagReco);
    data_.tagCutCode=~0x0;//disabled for now cuts_.getCutCode(*recoEle);
    data_.tagRecoDR = MathFuncs::calDeltaR(tagReco->p4(),tag.p4());
  }else{
    data_.tagReco.clear(); 
    data_.tagCutCode=~0x0;
    data_.tagRecoDR = 999.;
  }
  if(probeReco){
    data_.probeReco.fill(*probeReco);
    data_.probeCutCode=~0x0;//disabled for now cuts_.getCutCode(*recoEle);
    data_.probeRecoDR = MathFuncs::calDeltaR(probeReco->p4(),probe.p4());
  }else{
    data_.probeReco.clear(); 
    data_.probeCutCode=~0x0;
    data_.probeRecoDR = 999.;
  }
   
  tree_->Fill();
}

const SHElectron* EgHLTTagProbeTreeMaker::matchRecoEle_(const float eta,const float phi,const float maxDR,const SHEvent& event)
{
  float bestDR2=maxDR*maxDR;
  const SHElectron* bestEle=nullptr;
  for(int eleNr=0;eleNr<event.nrElectrons();eleNr++){
    const SHElectron* ele = event.getElectron(eleNr);
    if(!ele->hasTrack()) continue;
    float dR2 = MathFuncs::calDeltaR2(eta,phi,ele->eta(),ele->phi());
    if(dR2<bestDR2){
      bestEle=ele;
      bestDR2=dR2;
    }
  }
  return bestEle;
}

const SHMCParticle* EgHLTTagProbeTreeMaker::matchTruth_(const float eta,const float phi,const float maxDR,const SHEvent& event)
{
  auto mcParts = getMCPartsForMatch_(event);

  float bestDR2=maxDR*maxDR;
  const SHMCParticle* matchedPart=nullptr;
  for(auto mcPart : mcParts){
    float dR2 = MathFuncs::calDeltaR2(eta,phi,mcPart->eta(),mcPart->phi());
    if(dR2<bestDR2){
      matchedPart = mcPart;
      bestDR2=dR2;
    }
  }
  return matchedPart;
}

std::vector<const SHMCParticle*> EgHLTTagProbeTreeMaker::getMCPartsForMatch_(const SHEvent& event)
{
  std::vector<const SHMCParticle*> parts;
  const std::vector<int> pids={-11,11};
  const SHMCParticle* part=nullptr;
  int partNr=0;
  while(part!=nullptr || partNr==0){
    part=event.getGenInfo().getHardProcessPart(pids,SHGenInfo::PartStatus::PREFSR,partNr);
    partNr++;
    if(part) parts.push_back(part);
  }
  return parts;
}
