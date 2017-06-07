#include "SHarper/TrigNtup/interface/EgHLTBkgTreeMaker.hh"

#include "SHarper/TrigNtup/interface/TrigFuncs.hh"
#include "SHarper/TrigNtup/interface/CaloTools.hh"
#include "SHarper/SHNtupliser/interface/MathFuncs.hh"


EgHLTBkgTreeMaker::EgHLTBkgTreeMaker(const char* treeName,const char* treeTitle):
  minEtCut_(-1)
{
  tree_= new TTree(treeName,treeTitle);
  data_.createBranches(tree_);
}

void EgHLTBkgTreeMaker::fill(const SHEvent* event)
{
  for(const auto&trigObj :  event->getTrigSum().trigObjs()){
    if(trigObj.type()==SHTrigObj::PHOTON){
      fill_(trigObj,event);
    }//else{
      //      std::cout <<"trig obj type "<<trigObj.type()<<std::endl;
    //}
  }
}




void EgHLTBkgTreeMaker::fill_(const SHTrigObj& hlt,const SHEvent* event)
{

  const SHMCParticle* truth = matchTruth_(hlt.eta(),hlt.phi(),0.1,*event);
  const SHElectron* reco = matchRecoEle_(hlt.eta(),hlt.phi(),0.1,*event);
  
  data_.evt.fill(event);
  data_.weight = event->weight();
  data_.genWeight = event->getGenInfo().weight();
  data_.genPU.fill(*event);
  data_.evtTrigs = TrigBitsDef::getTrigCode(*event);
  data_.eleTrigs = TrigBitsDef::getTrigCode(*event,hlt.eta(),hlt.phi(),hlt.eta(),hlt.phi());
  data_.egHLT.fill(hlt);

  if(truth){
    data_.mc.fill(truth->p4(),truth->detEta());
    data_.eleTruthZ = truth->pos().Z();
    data_.truthRegion = CaloTools::region(truth->detEta());
    data_.pid = truth->pid();
    data_.truthDR = MathFuncs::calDeltaR(truth->p4(),hlt.p4());
  }else{
    data_.mc.clear();
    data_.eleTruthZ = -999;
    data_.truthRegion = -999;
    data_.pid = 0;
    data_.truthDR = 999;
  }
  
  if(reco){
    data_.ele.fill(*reco);
    data_.cutCode=~0x0;//disabled for now cuts_.getCutCode(*recoEle);
    data_.recoDR = MathFuncs::calDeltaR(reco->p4(),hlt.p4());
  }else{
    data_.ele.clear(); 
    data_.cutCode=~0x0;
    data_.recoDR = 999.;
  }
   
  tree_->Fill();
}

const SHElectron* EgHLTBkgTreeMaker::matchRecoEle_(const float eta,const float phi,const float maxDR,const SHEvent& event)
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

const SHMCParticle* EgHLTBkgTreeMaker::matchTruth_(const float eta,const float phi,const float maxDR,const SHEvent& event)
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

std::vector<const SHMCParticle*> EgHLTBkgTreeMaker::getMCPartsForMatch_(const SHEvent& event)
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
