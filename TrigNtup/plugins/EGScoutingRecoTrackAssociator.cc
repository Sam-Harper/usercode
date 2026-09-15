#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Scouting/interface/Run3ScoutingElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "SHarper/TrigNtup/interface/GsfTrackInfo.hh"

#include <string>
#include <vector>
#include <unordered_map>


class EGScoutingRecoTrackAssociator : public edm::stream::EDProducer<> {

private:

  
  
  edm::EDGetTokenT<std::vector<Run3ScoutingElectron>> scoutElesToken_;
  edm::EDGetTokenT<edm::View<reco::GsfElectron>> recoElesToken_;
  edm::EDPutTokenT<std::unordered_map<unsigned int,std::vector<GsfTrackInfo>>> mapToken_;
  EGScoutingRecoTrackAssociator(const EGScoutingRecoTrackAssociator& rhs)=delete;
  EGScoutingRecoTrackAssociator& operator=(const EGScoutingRecoTrackAssociator& rhs)=delete;

public:
  explicit EGScoutingRecoTrackAssociator(const edm::ParameterSet& iPara);
  virtual ~EGScoutingRecoTrackAssociator();
  
private:
  virtual void beginJob();
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endRun(edm::Run const& iRun, edm::EventSetup const&){}
  virtual void endJob();
  template<typename T>
  void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tagName){
    token = consumes<T>(iPara.getParameter<edm::InputTag>(tagName));
  }
  template<typename T>
  void setToken(std::vector<edm::EDGetTokenT<T> > & tokens,const edm::ParameterSet& iPara,const std::string& tagName){
    for(auto& tag: iPara.getParameter<std::vector<edm::InputTag> >(tagName)){
      tokens.push_back(consumes<T>(tag));
    }
  }
  const reco::GsfElectron* matchToReco(const Run3ScoutingElectron& scoutEle,const edm::View<reco::GsfElectron>& recoEles, float maxDR=0.05);
};



EGScoutingRecoTrackAssociator::EGScoutingRecoTrackAssociator(const edm::ParameterSet& iPara):
mapToken_{produces()}
{
  
  setToken(recoElesToken_,iPara,"recoElesTag");
  setToken(scoutElesToken_,iPara,"scoutElesTag");
  
  

}

EGScoutingRecoTrackAssociator::~EGScoutingRecoTrackAssociator()
{

}


void EGScoutingRecoTrackAssociator::beginJob()
{
  
} 

void EGScoutingRecoTrackAssociator::beginRun(const edm::Run& run,const edm::EventSetup& iSetup)
{ 
 
}

namespace {
  
  template<typename T> 
  std::vector<edm::Handle<T> > getHandle(const edm::Event& iEvent,const std::vector<edm::EDGetTokenT<T> >& tokens)
  {
    std::vector<edm::Handle<T> > handles;
    for(auto& token : tokens){
      edm::Handle<T> handle;
      iEvent.getByToken(token,handle);
      handles.emplace_back(std::move(handle));
    }
    return handles;
  }
}
  

void EGScoutingRecoTrackAssociator::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  auto scoutElesHandle = iEvent.getHandle(scoutElesToken_);
  auto recoElesHandle = iEvent.getHandle(recoElesToken_);

  std::unordered_map<unsigned int,std::vector<GsfTrackInfo>> trkMap;
  
  for(const auto& scoutEle : *scoutElesHandle){
    const reco::GsfElectron* recoEle = matchToReco(scoutEle,*recoElesHandle);
    //std::cout <<"scout ele pt "<<scoutEle.pt()<<" eta "<<scoutEle.eta()<<" phi "<<scoutEle.phi()<<" reco match "<<(recoEle ? "yes" : "no")<<std::endl;
    if(recoEle){
      //std::cout <<"reco ele pt "<<recoEle->pt()<<" eta "<<recoEle->superCluster()->eta()<<" phi "<<recoEle->superCluster()->phi()<<std::endl;

      std::vector<GsfTrackInfo> trkInfos;
      trkInfos.emplace_back(GsfTrackInfo(*recoEle->gsfTrack(),recoEle->fbrem()));

      for(const auto gsfTrk : recoEle->ambiguousGsfTracks()){
        trkInfos.emplace_back(*gsfTrk);
      }

      //std::cout <<"scout pt "<<scoutEle.pt()<<" bestTrk "

      trkMap.emplace(scoutEle.seedId(),std::move(trkInfos));

      //std::cout <<"reco ele gsf track pMode "<<pMode<<"etaMode "<<etaMode<<" phiMode "<<phiMode<<" qoverpModeError "<<qoverpModeError<<std::endl;
    }

  }
  iEvent.emplace(mapToken_,std::move(trkMap));
}

const reco::GsfElectron* EGScoutingRecoTrackAssociator::matchToReco(const Run3ScoutingElectron& scoutEle,const edm::View<reco::GsfElectron>& recoEles, float maxDR)
{
  const reco::GsfElectron* bestMatch=nullptr;
  float bestDR2=maxDR*maxDR;
  for(const auto& recoEle : recoEles){
    float dR2 = reco::deltaR2(recoEle.superCluster()->eta(),recoEle.superCluster()->phi(),scoutEle.eta(),scoutEle.phi());
    if(dR2<bestDR2){
      bestMatch = &recoEle;
      bestDR2 = dR2;
    }
  }
  return bestMatch;
}



void EGScoutingRecoTrackAssociator::endJob()
{ 

}


  


//define this as a plug-in
DEFINE_FWK_MODULE(EGScoutingRecoTrackAssociator);
