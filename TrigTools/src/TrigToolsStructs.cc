#include "SHarper/TrigTools/interface/TrigToolsStructs.h"

#include "FWCore/Framework/interface/Event.h"

void trigtools::PathData::fill(const std::vector<TLorentzVector>& trigP4s)
{
  //std::cout <<"start "<<trigInfo<<std::endl;
  
  std::vector<TLorentzVector> etAll;
  std::vector<TLorentzVector> etEB;
  std::vector<TLorentzVector> etEE;

  nrPass = trigP4s.size();
  sumEt=0.;
  sumE=0.;
  sumHT=0.;
  sumMHT=0.;
  
  float mhtX = 0.;
  float mhtY = 0.;
  for(size_t objNr=0;objNr<trigP4s.size();objNr++){ 
    //std::cout <<objNr<<"/"<<nrPass<<std::endl;
    const TLorentzVector& p4 = trigP4s[objNr];
    etAll.push_back(p4);
    sumEt+=p4.Et();
    sumE+=p4.E();
    if(fabs(p4.Eta())<3.0){
      sumHT+=p4.Pt();      
      mhtX-=p4.Px();
      mhtY-=p4.Py();
    }

    if(fabs(p4.Eta())<1.5) etEB.push_back(p4);
    else etEE.push_back(p4);
  }
  sumMHT=sqrt(mhtX*mhtX+mhtY*mhtY);

  std::sort(etAll.begin(),etAll.end(),EtP4Sorter());
  std::sort(etEB.begin(),etEB.end(),EtP4Sorter()); 
  std::sort(etEE.begin(),etEE.end(),EtP4Sorter()); 
  
 

  p4First = etAll.size()>=1 ? etAll[etAll.size()-1] : -1;
  p4Second = etAll.size()>=2 ? etAll[etAll.size()-2] : -1;
  p4Third = etAll.size()>=3 ? etAll[etAll.size()-3] : -1;
  p4Fourth = etAll.size()>=4 ? etAll[etAll.size()-4] : -1;
  p4Fifth = etAll.size()>=5 ? etAll[etAll.size()-5] : -1;
  p4Sixth = etAll.size()>=6 ? etAll[etAll.size()-6] : -1;
  p4FirstEB = etEB.size()>=1 ? etEB[etEB.size()-1] : -1;
  p4SecondEB = etEB.size()>=2 ? etEB[etEB.size()-2] : -1;
  p4FirstEE = etEE.size()>=1 ? etEE[etEE.size()-1] : -1;
  p4SecondEE = etEE.size()>=2 ? etEE[etEE.size()-2] : -1;
  
  // std::cout <<"done"<<std::endl;

}

void trigtools::EvtInfoStruct::fill(const edm::Event& event)
{
  runnr = event.id().run();
  lumiSec = event.luminosityBlock();
  eventnr = event.id().event();
  bx = event.bunchCrossing();
}
