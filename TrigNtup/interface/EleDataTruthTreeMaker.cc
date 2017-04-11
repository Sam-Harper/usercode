#include "AnaTrees/EleDataTruthTreeMaker.hh"

#include "SHUtility/CutCodes.hh"
#include "SHUtility/TrigFuncs.hh"


EleDataTruthTreeMaker::EleDataTruthTreeMaker(const char* treeName,const char* treeTitle,
					     const EleCutter &cuts):
  cuts_(cuts),minEtCut_(-1)
{
  tree_= new TTree(treeName,treeTitle);
  data_.createBranches(tree_);
}

void EleDataTruthTreeMaker::fill(const SHEvent* event)
{
  
  const SHMCParticle* ele1Truth = event->getGenInfo().getHardProcessPart(11,SHGenInfo::PartStatus::PREFSR); 
  const SHMCParticle* ele2Truth = event->getGenInfo().getHardProcessPart(-11,SHGenInfo::PartStatus::PREFSR);

  if(ele1Truth && ele2Truth){

    std::vector<const SHElectron*> eles;
    for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
      eles.push_back(event->getElectron(eleNr));
    }
    
    const SHElectron* ele1,*ele2;
    matchRecoToTruth(*event,eles,ele1,ele2);
   
    fill_(event,*ele1Truth,ele1);
    fill_(event,*ele2Truth,ele2);
  }else if(ele1Truth){
    const SHElectron* eleReco = getBestMatchedRecoEle(*event,ele1Truth);
    fill_(event,*ele1Truth,eleReco);
  }else if(ele2Truth){
    const SHElectron* eleReco = getBestMatchedRecoEle(*event,ele2Truth);
    fill_(event,*ele2Truth,eleReco);
  }
}



void EleDataTruthTreeMaker::fill_(const SHEvent* event,const SHMCParticle& truthEle,const SHElectron* recoEle)
{
  data_.eleTruthNrgy = truthEle.p4().E();
  data_.eleTruthEt = truthEle.p4().Et();
  data_.eleTruthEta = truthEle.p4().Eta();
  data_.eleTruthPhi = truthEle.p4().Phi();
  data_.eleTruthDetEta = truthEle.detEta();
  data_.eleTruthZ = truthEle.pos().Z();
  data_.pid = truthEle.pid();
  data_.evt.fill(event);
  data_.weight = event->weight();
  data_.genWeight = event->getGenInfo().weight();
  data_.genPU.fill(*event);
  if(recoEle!=NULL){
    data_.truthDR = MathFuncs::calDeltaR(truthEle.p4(),recoEle->p4());
    data_.ele.fill(*recoEle);
    data_.cutCode=cuts_.getCutCode(*recoEle);
  }else{
    data_.truthDR = 999.; 
    data_.ele.clear();
    data_.cutCode=~0x0;
  }
   
  data_.eleTrigs = TrigBitsDef::getTrigCode(*event,data_.eleTruthDetEta,data_.eleTruthPhi,data_.eleTruthEta,data_.eleTruthPhi);
  data_.egHLT.clear();
  data_.egHLTUS.clear();
  auto egObjs = TrigFuncs::getEGTrigObj(data_.eleTruthDetEta,data_.eleTruthPhi,event->getTrigSum());
  if(egObjs.first) data_.egHLT.fill(*egObjs.first);
  if(egObjs.second) data_.egHLTUS.fill(*egObjs.second,"Unseeded");
  
  tree_->Fill();
  

}



void EleDataTruthTreeMaker::matchRecoToTruth(const SHEvent& event,std::vector<const SHElectron*>& eles,const SHElectron* & ele1,const SHElectron* & ele2)
{
  ele1=NULL;
  ele2=NULL;

  const SHMCParticle* ele1Truth = event.getGenInfo().getHardProcessPart(11,SHGenInfo::PartStatus::PREFSR);
  const SHMCParticle* ele2Truth = event.getGenInfo().getHardProcessPart(-11,SHGenInfo::PartStatus::PREFSR);

  double ele1BestDR=999.,ele2BestDR=999.;
  int ele1BestMatch= getBestMatch(eles,ele1Truth->p4(),ele1BestDR);
  int ele2BestMatch= getBestMatch(eles,ele2Truth->p4(),ele2BestDR);
  if(ele1BestMatch==ele2BestMatch){ //damn, matched to same ele
    if(ele1BestDR<ele2BestDR) { //ele1 has better match, redo ele2
      ele2BestMatch = getBestMatch(eles,ele2Truth->p4(),ele2BestDR,ele1BestMatch);
    }else{
      ele1BestMatch = getBestMatch(eles,ele1Truth->p4(),ele1BestDR,ele2BestMatch);
    } 
  }  
    
  if(ele1BestMatch!=-1 && ele1BestMatch<static_cast<int>(eles.size())) ele1 = eles[ele1BestMatch];
  if(ele2BestMatch!=-1 && ele2BestMatch<static_cast<int>(eles.size())) ele2 = eles[ele2BestMatch];
 

}

int EleDataTruthTreeMaker::getBestMatch(const std::vector<const SHElectron*>& eles,const TLorentzVector& p4,double& bestDeltaR,int otherEleIndx)
{
  bestDeltaR = 999;
  int bestEleIndx=-1;
  
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    if(otherEleIndx==static_cast<int>(eleNr)) continue;
    double deltaR =MathFuncs::calDeltaR(eles[eleNr]->p4(),p4);
    if(deltaR<bestDeltaR){
      bestEleIndx = eleNr;
      bestDeltaR = deltaR;
    }
  }
  return bestEleIndx;
}

const SHElectron* EleDataTruthTreeMaker::getBestMatchedRecoEle(const SHEvent& event,const SHMCParticle* mcPart)
{

  float mcEta = mcPart->p4().Eta();
  float mcPhi = mcPart->p4().Phi();

  float bestDR2=999;
  const SHElectron* bestEle=nullptr;
  for(int eleNr=0;eleNr<event.nrElectrons();eleNr++){
    const SHElectron* ele = event.getElectron(eleNr);
    float dR2 = MathFuncs::calDeltaR2(mcEta,mcPhi,ele->eta(),ele->phi());
    if(dR2<bestDR2){
      bestEle=ele;
      bestDR2=dR2;
    }
  }
  return bestEle;
}
