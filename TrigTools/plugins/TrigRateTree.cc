#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h" 

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TTree.h"
#include <iostream>

class TrigRateTree : public edm::EDAnalyzer {
  edm::EDGetTokenT<edm::MergeableCounter> nrEventsRunToken_;
  edm::EDGetTokenT<edm::MergeableCounter> nrEventsRunPostPUFilterToken_;
  edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
  edm::EDGetTokenT<edm::TriggerResults> trigResultsPUFilterToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupInfoToken_;
  int datasetCode_;

  unsigned int nrEventsRunSum_;
  unsigned int nrEventsRunPostPUFilterSum_;
  std::string pileupFilterPath_;
  std::vector<std::string> triggerPaths_;

  static constexpr int kMaxNrPU_ = 70;

  std::vector<std::array<int,kMaxNrPU_+1> > hltCounts_;
  std::vector<std::array<int,kMaxNrPU_+1> > hltCountsPostPUFilter_;
  
  
  
public:
  
  TrigRateTree(const edm::ParameterSet& pset):nrEventsRunSum_(0),nrEventsRunPostPUFilterSum_(0){
    nrEventsRunToken_ = consumes<edm::MergeableCounter,edm::InRun>(pset.getParameter<edm::InputTag>("nrEventsInRun"));
    nrEventsRunPostPUFilterToken_ = consumes<edm::MergeableCounter,edm::InRun>(pset.getParameter<edm::InputTag>("nrEventsInRunPostPUFilter"));
    trigResultsToken_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("triggerResults"));
    trigResultsPUFilterToken_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("triggerResultsPUFilter"));
    pileupInfoToken_ = consumes<std::vector<PileupSummaryInfo> >(pset.getParameter<edm::InputTag>("pileupInfo"));
    pileupFilterPath_ = pset.getParameter<std::string>("pileupFilterPath");
    triggerPaths_ = pset.getParameter<std::vector<std::string> >("triggerPaths");
    datasetCode_ = pset.getParameter<int>("datasetCode");

    hltCounts_.resize(triggerPaths_.size());
    hltCountsPostPUFilter_.resize(triggerPaths_.size());
    
    
  }

  
  void analyze(const edm::Event& event,const edm::EventSetup& setup) override;

  void beginRun(const edm::Run& run,const edm::EventSetup& setup ) override{}
   
  void endRun(const edm::Run& run,const edm::EventSetup& setup ) override{
    edm::Handle<edm::MergeableCounter> nrEventsHandle;
    run.getByToken(nrEventsRunToken_,nrEventsHandle);
    edm::Handle<edm::MergeableCounter> nrEventsPostPUFilterHandle;
    run.getByToken(nrEventsRunPostPUFilterToken_,nrEventsPostPUFilterHandle);
    nrEventsRunSum_+=nrEventsHandle->value; 
    nrEventsRunPostPUFilterSum_+=nrEventsPostPUFilterHandle->value; 
  }
 
  void endJob()override;

  bool checkPUFilter(const edm::Event& event);

};


namespace{
  
  template<typename T> edm::Handle<T> getHandle(const edm::Event& event,const edm::EDGetTokenT<T>& token){
    edm::Handle<T> handle;
    event.getByToken(token,handle);
    return handle;
  }
}

void TrigRateTree::analyze(const edm::Event& event,const edm::EventSetup& setup)
{

  auto trigResults = getHandle(event,trigResultsToken_);
  auto trigNames = event.triggerNames(*trigResults);
  auto puInfo = getHandle(event,pileupInfoToken_);
  int nrTruePU = puInfo->empty() ? 0 : puInfo->front().getTrueNumInteractions();
  if(nrTruePU>kMaxNrPU_){
    throw cms::Exception("ConfigError") <<" # true pileup is "<<nrTruePU<<" and max allowed is "<<kMaxNrPU_<<std::endl;
  }
  bool passPUFilter = checkPUFilter(event);
  
  for(size_t pathNr=0;pathNr<triggerPaths_.size();pathNr++){
    size_t pathIndex = trigNames.triggerIndex(triggerPaths_[pathNr]);
    if(pathIndex<trigResults->size() && trigResults->accept(pathIndex)){
      hltCounts_[pathNr][nrTruePU]++;
      if(passPUFilter) hltCountsPostPUFilter_[pathNr][nrTruePU]++;
    }
  }
	 
}

void TrigRateTree::endJob()
{ 
  edm::Service<TFileService> fs;
  fs->file().cd();
  TTree* tree = new TTree("rateTree","rateTree");
  tree->Branch("datasetCode",&datasetCode_,"datasetCode/I");
  tree->Branch("nrTot",&nrEventsRunSum_,"nrTot/I");
  tree->Branch("nrTotPostPUFilter",&nrEventsRunPostPUFilterSum_,"nrTotPostPUFilter/I");

  std::vector<int> nrPass(triggerPaths_.size(),0);
  std::vector<int> nrPassPostPUFilter(triggerPaths_.size(),0);
  

  for(size_t pathNr=0;pathNr<triggerPaths_.size();pathNr++){
    const auto& pathName = triggerPaths_[pathNr];
    nrPass[pathNr]= std::accumulate(hltCounts_[pathNr].begin(),hltCounts_[pathNr].end(),0);
    nrPassPostPUFilter[pathNr] = std::accumulate(hltCountsPostPUFilter_[pathNr].begin(),hltCountsPostPUFilter_[pathNr].end(),0);

    tree->Branch((pathName+"_pass").c_str(),&nrPass[pathNr],"nrPass/I");
    tree->Branch((pathName+"_passPostPUFilter").c_str(),&nrPassPostPUFilter[pathNr],"nrPass/I");
    tree->Branch((pathName+"_vsPU").c_str(),hltCounts_[pathNr].data(),("counts["+std::to_string(kMaxNrPU_+1)+"]/I").c_str());
    tree->Branch((pathName+"_vsPUPostPUFilter").c_str(),hltCountsPostPUFilter_[pathNr].data(),("counts["+std::to_string(kMaxNrPU_+1)+"]/I").c_str());    
  }
  tree->Fill();
  
}


bool TrigRateTree::checkPUFilter(const edm::Event& event) 
{  

  auto trigResults = getHandle(event,trigResultsPUFilterToken_);
  auto trigNames = event.triggerNames(*trigResults);
  
  size_t pathIndex = trigNames.triggerIndex(pileupFilterPath_);
  if(pathIndex<trigResults->size() && trigResults->accept(pathIndex)) return true;
  else return false;
}



DEFINE_FWK_MODULE(TrigRateTree);
