

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/TriggerResults.h"


#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "SHarper/TrigTools/interface/TrigToolsStructs.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class RunLumiKey {
private:
  unsigned int runnr_;
  unsigned int lumiSec_;
public:

  RunLumiKey():runnr_(0),lumiSec_(0){}
  RunLumiKey(unsigned int runnr,unsigned int lumiSec):runnr_(runnr),lumiSec_(lumiSec){}
  bool operator<(const RunLumiKey& rhs)const{
    if(runnr_<rhs.runnr_) return true;
    else if(runnr_>rhs.runnr_) return false;
    else return lumiSec_<rhs.lumiSec_;
  }
  bool operator==(const RunLumiKey& rhs)const{
    return runnr_==rhs.runnr_ && lumiSec_==rhs.lumiSec_;
  }
  bool operator!=(const RunLumiKey& rhs)const{return !(*this==rhs);}

  unsigned int runnr()const{return runnr_;}
  unsigned int lumiSec()const{return lumiSec_;}
  
};

class TrigCounts {
public:
  struct TrigCountsData {
    std::vector<unsigned int> counts;
    std::vector<double> weights;
    std::vector<double> weightsSq;
    TrigCountsData(size_t size):counts(size,0),weights(size,0.),weightsSq(size,0.){}
  };
private:

  std::map<RunLumiKey,TrigCountsData> data_;
  std::vector<std::string> pathNames_;
public:
  
  TrigCounts(){}
  
  void init(std::vector<std::string> pathNames){
    pathNames_=std::move(pathNames);
  }
  
  void fill(const RunLumiKey& key,const edm::TriggerNames& trigNames,
	    const edm::TriggerResults& trigResults,const HLTConfigProvider& hltConfig);
	    
  const std::map<RunLumiKey,TrigCountsData>& data()const{return data_;}
  const std::vector<std::string>& pathNames()const{return pathNames_;}
  std::vector<std::string>& pathNames(){return pathNames_;}

};


class TrigResultTreeMakerV3 : public edm::EDAnalyzer { 

private:
   
  edm::InputTag trigResultsTag_;
  edm::InputTag trigResultsP5Tag_;
  edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
  edm::EDGetTokenT<edm::TriggerResults> trigResultsP5Token_;
  
  std::unique_ptr<HLTPrescaleProvider> hltConfig_;
  std::unique_ptr<HLTPrescaleProvider> hltConfigP5_;
 
  
  TrigCounts trigCounts_;
  TrigCounts trigCountsP5_;

  int datasetCode_;
  int psColumn_;
  int hltPhysicsPS_;
  std::string refTrigger_;
  TTree* tree_; //not owned by us


public:
  explicit TrigResultTreeMakerV3(const edm::ParameterSet& iPara);
  ~TrigResultTreeMakerV3(){}

  TrigResultTreeMakerV3(const TrigResultTreeMakerV3& rhs)=delete;
  TrigResultTreeMakerV3& operator=(const TrigResultTreeMakerV3& rhs)=delete;

 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob();
  
  

};


TrigResultTreeMakerV3::TrigResultTreeMakerV3(const edm::ParameterSet& iPara):tree_(nullptr)
{
  trigResultsTag_ = iPara.getParameter<edm::InputTag>("trigResultsTag");
  trigResultsToken_ = consumes<edm::TriggerResults>(trigResultsTag_);
  trigResultsP5Tag_ = iPara.getParameter<edm::InputTag>("trigResultsP5Tag");
  trigResultsP5Token_ = consumes<edm::TriggerResults>(trigResultsP5Tag_);

  hltConfig_ = std::make_unique<HLTPrescaleProvider>(iPara,consumesCollector(),*this);
  hltConfigP5_ = std::make_unique<HLTPrescaleProvider>(iPara,consumesCollector(),*this);

  datasetCode_ = iPara.getParameter<int>("datasetCode");

}

void TrigResultTreeMakerV3::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  std::cout <<"begining run "<<std::endl;
  bool changed=false;
  hltConfig_->init(run,setup,trigResultsTag_.process(),changed); //as we need the orginal HLT config...
  std::cout <<"new table name "<<hltConfig_->hltConfigProvider().tableName()<<std::endl;

  bool changedP5=false;
  hltConfigP5_->init(run,setup,trigResultsP5Tag_.process(),changed); //as we need the orginal HLT config...
  std::cout <<"new table name "<<hltConfigP5_->hltConfigProvider().tableName()<<std::endl;
  
  
  if((changed || changedP5) && ! trigCounts_.pathNames().empty()){ 
    std::cout <<"TrigResultTreeMakerV3::beginRun warning HLT menu has changed new: "<<changed<<" p5 "<<changedP5<<std::endl;
  }
  if(trigCounts_.pathNames().empty()){ //initialising hlt paths names
    trigCounts_.pathNames() = hltConfig_->hltConfigProvider().triggerNames();
    trigCountsP5_.pathNames() = hltConfigP5_->hltConfigProvider().triggerNames();
  }
  
}


void TrigCounts::fill(const RunLumiKey& key,const edm::TriggerNames& trigNames,
		      const edm::TriggerResults& trigResults,const HLTConfigProvider& hltConfig)
{
  const int psColumn=0;
  
  std::vector<unsigned int> defaultPSes(hltConfig.prescaleSize(),1);

  auto& countsData = data_.insert({key,TrigCountsData(pathNames_.size())}).first->second;

  for(size_t pathNr=0;pathNr<pathNames_.size();pathNr++){
    size_t pathIndex = trigNames.triggerIndex(pathNames_[pathNr]);
    if(pathIndex<trigResults.size() &&  trigResults.accept(pathIndex)){
	
      auto psTblEntry = hltConfig.prescaleTable().find(pathNames_[pathNr]);
      const std::vector<unsigned int>& preScales = psTblEntry!=hltConfig.prescaleTable().end() ? 
	psTblEntry->second : defaultPSes;
      int preScale=1;
      if(static_cast<size_t>(psColumn)<preScales.size()) preScale = preScales[psColumn];
      //	else std::cout <<"for path "<<pathNames_[pathNr]<<" column "<<psColumn<<" higher than number of ps columns "<<preScales.size();
      
      countsData.counts[pathNr]++;
      countsData.weights[pathNr]+=preScale;
      countsData.weightsSq[pathNr]+=preScale*preScale;
    }
  }
}

void TrigResultTreeMakerV3::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  edm::Handle<edm::TriggerResults> trigResultsHandle;
  iEvent.getByToken(trigResultsToken_,trigResultsHandle);
  edm::Handle<edm::TriggerResults> trigResultsP5Handle;
  iEvent.getByToken(trigResultsP5Token_,trigResultsP5Handle);
  
  const edm::TriggerResults& trigResults = *trigResultsHandle;
  const edm::TriggerNames& trigNames = iEvent.triggerNames(trigResults);  
  const edm::TriggerResults& trigResultsP5 = *trigResultsP5Handle;
  const edm::TriggerNames& trigNamesP5 = iEvent.triggerNames(trigResultsP5);  
  

  psColumn_= 5;//hltConfigP5_->prescaleSet(iEvent,iSetup);
  hltPhysicsPS_ =75;// = hltConfigP5_->prescaleValues(iEvent,iSetup,refTrigger_).second;
  
  RunLumiKey key(iEvent.id().run(),iEvent.luminosityBlock());
  trigCounts_.fill(key,trigNames,trigResults,hltConfig_->hltConfigProvider());
  trigCountsP5_.fill(key,trigNamesP5,trigResultsP5,hltConfigP5_->hltConfigProvider()); 
}

struct TrigCountsForTree {
  std::vector<unsigned int> counts;
  std::vector<float> countsPS;
  std::vector<float> countsPSSq;
  
  TrigCountsForTree(size_t size):counts(size),countsPS(size),countsPSSq(size){}
  
  void fill(const TrigCounts::TrigCountsData& data)
  {
    for(size_t i=0;i<data.counts.size();i++) counts[i]=data.counts[i];
    for(size_t i=0;i<data.weights.size();i++) countsPS[i]=data.weights[i];
    for(size_t i=0;i<data.weightsSq.size();i++) countsPSSq[i]=data.weightsSq[i];
  }
  void clear(){
    for(auto& val : counts) val=0;
    for(auto& val : countsPS) val=0.;
    for(auto& val : countsPSSq) val=0.;
  }
    
  void createBranches(TTree* tree,const std::string& tag){
    tree->Branch(("trigCounts"+tag).c_str(),&counts[0],("trigCounts"+tag+"["+std::to_string(counts.size())+"]/i").c_str());
    tree->Branch(("trigCountsPS"+tag).c_str(),&countsPS[0],("trigCountsPS"+tag+"["+std::to_string(countsPS.size())+"]/F").c_str()); 
    tree->Branch(("trigCountsPSSq"+tag).c_str(),&countsPSSq[0],("trigCountsPSSq"+tag+"["+std::to_string(countsPSSq.size())+"]/F").c_str()); 
  }
    

};

void TrigResultTreeMakerV3::endJob()
{
  edm::Service<TFileService> fs;
  TFile* file = &fs->file();
  file->cd();
   
  int runnr;
  int lumiSec;
  int datasetCode=datasetCode_;

  TrigCountsForTree trigCountsForTree(trigCounts_.pathNames().size());
  TrigCountsForTree trigCountsForTreeP5(trigCountsP5_.pathNames().size());

  TTree* tree = new TTree("trigResultV3Tree","tree");
  tree->Branch("runnr",&runnr,"runnr/I");
  tree->Branch("lumiSec",&lumiSec,"lumiSec/I");
  tree->Branch("datasetCode",&datasetCode,"datasetCode/I");
  trigCountsForTree.createBranches(tree,"");
  trigCountsForTreeP5.createBranches(tree,"P5");
  
  for(const auto& entry : trigCounts_.data()){
    const RunLumiKey& key = entry.first;
    auto p5It = trigCountsP5_.data().find(key);
    runnr=key.runnr();
    lumiSec=key.lumiSec();
    
    trigCountsForTree.fill(entry.second);
    if(p5It !=trigCountsP5_.data().end()){
      trigCountsForTreeP5.fill(p5It->second);
    }else trigCountsForTreeP5.clear();
    tree->Fill();
  }


 
  file->WriteObject(&trigCounts_.pathNames(),"trigPathNames");
  file->WriteObject(&trigCountsP5_.pathNames(),"trigPathNamesP5");
}

 



//define this as a plug-in
DEFINE_FWK_MODULE(TrigResultTreeMakerV3);
