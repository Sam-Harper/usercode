
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

#include "TTree.h"

class PFElePassingChecker : public edm::EDAnalyzer {

private:

  edm::InputTag pfCandTag_;
  edm::InputTag gsfEleTag_;
  
public:
  explicit PFElePassingChecker(const edm::ParameterSet& para);
  ~PFElePassingChecker(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();


  static bool eleMatch(const reco::GsfElectron* gsfEle,const reco::PFCandidate* pfEle);
 
};

PFElePassingChecker::PFElePassingChecker(const edm::ParameterSet& para)

{
  pfCandTag_=para.getParameter<edm::InputTag>("pfCands");
  gsfEleTag_=para.getParameter<edm::InputTag>("gsfEles");
  
  
}

void PFElePassingChecker::beginJob()
{
  
}

void PFElePassingChecker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 

  
  edm::Handle<reco::GsfElectronCollection> eleHandle; 
  edm::Handle<reco::PFCandidateCollection> pfCandHandle;
  
  iEvent.getByLabel(gsfEleTag_,eleHandle);
  iEvent.getByLabel(pfCandTag_,pfCandHandle);
  
  typedef std::pair<reco::GsfTrackRef,const reco::PFCandidate*> GsfTrkPair;
  typedef TempFuncs::PairSortBy1st<reco::GsfTrackRef,const reco::PFCandidate*> GsfTrkPairSort;
  std::vector<GsfTrkPair> gsfPFMap;
  
  for(auto pfIt=pfCandHandle->begin();pfIt!=pfCandHandle->end();++pfIt){
    if(pfIt->gsfTrackRef().isNonnull() && abs(pfIt->pdgId())==11){
      gsfPFMap.push_back(GsfTrkPair(pfIt->gsfTrackRef(),&(*pfIt)));
    }
  }
  std::sort(gsfPFMap.begin(),gsfPFMap.end(),GsfTrkPairSort());
  
  //std::cout <<"nrElectrons "<<eleHandle->size()<<std::endl;
  for(auto eleIt=eleHandle->begin();eleIt!=eleHandle->end();++eleIt){
 
    auto result = std::equal_range(gsfPFMap.begin(),gsfPFMap.end(),eleIt->gsfTrack(),GsfTrkPairSort());
    int nrFound = std::distance(result.first,result.second);  

    bool badEle=false;
    if(eleIt->mvaOutput().status==3 && nrFound!=1) badEle=true;
    if(eleIt->mvaOutput().status==4 && nrFound!=0) badEle=true;
    if(nrFound>1) badEle=true;
    if(nrFound==1 && eleMatch(&(*eleIt),result.first->second)==false) badEle=true;

    
    if(badEle) std::cout <<"ele "<<std::distance(eleHandle->begin(),eleIt)<<" / "<<eleHandle->size()<<" et "<<eleIt->et()<<" eta "<<eleIt->eta()<<" phi "<<eleIt->phi()<<" mva  status "<<eleIt->mvaOutput().status<<" nrPFCands found "<<nrFound;
    
    if(nrFound>1){
      if(badEle) std::cout <<" failed to uniquely ID ele, found "<<nrFound<<std::endl;
    }else if(nrFound==1){
      if(badEle) std::cout <<" pf et "<<result.first->second->et()<<" eta "<<result.first->second->eta()<<" phi "<<result.first->second->phi()<<std::endl;
    }else if(badEle) std::cout <<std::endl;

  }



}

bool PFElePassingChecker::eleMatch(const reco::GsfElectron* gsfEle,const reco::PFCandidate* pfEle)
{
  if(fabs(gsfEle->et()-pfEle->et())<0.001 &&
     fabs(gsfEle->eta()-pfEle->eta()) <0.001 &&
     fabs(gsfEle->phi()-pfEle->phi()) <0.001) return true;  //we are checking identical within rounding errors, hence pi dir sub is prob fine...
  else return false;
}



void PFElePassingChecker::endJob()
{ 
 
  
}  



DEFINE_FWK_MODULE(PFElePassingChecker);
