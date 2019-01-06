#include "SHarper/SHNtupliser/interface/SHEventTreeData.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/Registry.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"

#include <openssl/md5.h>
#include <iomanip>

void SHEventTreeData::BranchData::setup(const edm::ParameterSet& iPara)
{
  addPFCands=iPara.getParameter<bool>("addPFCands");
  addPFClusters=iPara.getParameter<bool>("addPFClusters");
  addCaloTowers=iPara.getParameter<bool>("addCaloTowers");
  addCaloHits=iPara.getParameter<bool>("addCaloHits");
  addIsolTrks=iPara.getParameter<bool>("addIsolTrks");
  addPreShowerClusters=iPara.getParameter<bool>("addPreShowerClusters");
  addGenInfo=iPara.getParameter<bool>("addGenInfo");
  addPUInfo=iPara.getParameter<bool>("addPUInfo");
  addTrigSum=iPara.getParameter<bool>("addTrigSum");
  addMet=iPara.getParameter<bool>("addMet");
  addJets=iPara.getParameter<bool>("addJets");
  addMuons=iPara.getParameter<bool>("addMuons");
  addSuperClus=iPara.getParameter<bool>("addSuperClus");
  addEles=iPara.getParameter<bool>("addEles");
  addHLTDebug=iPara.getParameter<bool>("addHLTDebug");
  addMCParts=iPara.getParameter<bool>("addMCParts");
  addPDFWeights=iPara.getParameter<bool>("addPDFWeights");
  addGainSwitchInfo=iPara.getParameter<bool>("addGainSwitchInfo");
  addJetMETExtra=iPara.getParameter<bool>("addJetMETExtra");
  addEleUserData=iPara.getParameter<bool>("addEleUserData");
  fillTrkIsolFromUserData=iPara.getParameter<bool>("fillTrkIsolFromUserData");
  trkIsolUserDataName=iPara.getParameter<std::string>("trkIsolUserDataName");
  
  filterIsolTrks=iPara.getParameter<bool>("filterIsolTrks");
  filterEcalHits=iPara.getParameter<bool>("filterEcalHits");
  filterHcalHits=iPara.getParameter<bool>("filterHcalHits");
  filterCaloTowers=iPara.getParameter<bool>("filterCaloTowers");
}

SHEventTreeData::SHEventTreeData(SHEvent* & event):
  tree_(nullptr),
  event_(event),
  shPUSum_(nullptr),
  shPFCands_(nullptr),
  shPFClusters_(nullptr),
  shCaloTowers_(nullptr),
  shCaloHits_(nullptr),
  shIsolTrks_(nullptr),
  shPreShowerClusters_(nullptr),
  shGenInfo_(nullptr), 
  shTrigSum_(nullptr),
  shGSInfo_(nullptr),
  shJetMETExtra_(nullptr)
{
  if(event_) setMemLocs();

}

void SHEventTreeData::setMemLocs()
{
  shPUSum_ = &(event_->getPUSum());
  shPFCands_ = &(event_->getPFCands());
  shPFClusters_ = &(event_->getPFClusters());
  shCaloTowers_ = &(event_->getCaloTowers());
  shCaloHits_ = &(event_->getCaloHits());
  shIsolTrks_ = &(event_->getIsolTrks());
  shPreShowerClusters_ = &(event_->getPreShowerClusters());
  shGenInfo_ = &(event_->getGenInfo()); 
  shTrigSum_ = &(event_->getTrigSum());
  shGSInfo_ = &(event_->getGSInfo());
  shJetMETExtra_ = &(event_->getJetMETExtra());
}

void SHEventTreeData::makeTree(const std::string& name)
{  
  setMemLocs();

  tree_= new TTree(name.c_str(),"Event Tree");
  int splitLevel=2;
  tree_->SetCacheSize(1024*1024*100);
					       
  tree_->Branch("EventBranch",event_->GetName(),&event_,32000,splitLevel);
  if(branches_.addPFCands) tree_->Branch("PFCandsBranch","SHPFCandContainer",&shPFCands_,32000,splitLevel);
  if(branches_.addPFClusters) tree_->Branch("PFClustersBranch","SHPFClusterContainer",&shPFClusters_,32000,splitLevel);
  if(branches_.addCaloTowers) tree_->Branch("CaloTowersBranch","SHCaloTowerContainer",&shCaloTowers_,32000,splitLevel);
  if(branches_.addCaloHits) tree_->Branch("CaloHitsBranch","SHCaloHitContainer",&shCaloHits_,32000,splitLevel);
  if(branches_.addIsolTrks) tree_->Branch("IsolTrksBranch","TClonesArray",&shIsolTrks_,32000,splitLevel);
  if(branches_.addPreShowerClusters) tree_->Branch("PreShowerClustersBranch","TClonesArray",&shPreShowerClusters_,32000,splitLevel);
  if(branches_.addGenInfo) tree_->Branch("GenInfoBranch","SHGenInfo",&shGenInfo_,32000,splitLevel);
  if(branches_.addPUInfo) tree_->Branch("PUInfoBranch","SHPileUpSummary",&shPUSum_,32000,splitLevel);
  if(branches_.addTrigSum) tree_->Branch("TrigSummaryBranch","SHTrigSummary",&shTrigSum_,32000,splitLevel);
  if(branches_.addGainSwitchInfo) tree_->Branch("GainSwitchInfoBranch","SHGainSwitchInfo",&shGSInfo_,32000,splitLevel);
  if(branches_.addJetMETExtra) tree_->Branch("JetMETExtraBranch","SHJetMETExtra",&shJetMETExtra_,32000,splitLevel);

  
  
}

void SHEventTreeData::fill()
{
  //menu is not already stored in the manager, add it (first check we have a menu to  store though)
  if(shTrigSum_->hltMenu() && 
     !trigMenuMgr_.getHLT(shTrigSum_->menuName(),shTrigSum_->processName()).valid()) {
    trigMenuMgr_.add(*shTrigSum_->hltMenu());
  }
  if(shTrigSum_->l1Menu() && 
     !trigMenuMgr_.getL1(shTrigSum_->l1MenuName()).valid()) {
    trigMenuMgr_.add(*shTrigSum_->l1Menu());
  }
  trigMenuMgr_.write(tree_);
  shTrigSum_->clearMenuData(); 
  event_->setConfigMD5SumStr(shEventProv_.md5SumStr());
  tree_->Fill();
}

void SHEventTreeData::setup(const edm::ParameterSet& iPara)
{
  branches_.setup(iPara);
  shEventProv_.setSHNtupConfig(iPara.dump());
  auto vids = iPara.getParameter<std::vector<edm::InputTag>>("vidBits");
  std::vector<std::string> vidNames;
  for(const auto& vid : vids) vidNames.push_back(vid.instance());
  shEventProv_.setVIDNames(std::move(vidNames));
}

void SHEventTreeData::runSetup(const edm::Run& run,const edm::EventSetup& iSetup)
{
  //currently we only care about the current process which is always the same
  //so check its not filled and if its not, we fill it once
  //this will change if we decide to save the provenance of previous jobs
  if(shEventProv_.processName().empty()){
     auto processHist = run.processHistory();
     if(processHist.empty()){
       throw cms::Exception("ProvError") <<" warning run "<<run.id()<<" has no history"<<std::endl;
     }
     //we want the last process name
     shEventProv_.setProcessName(processHist[processHist.size()-1].processName());
     edm::ProcessConfiguration processConfiguration;
     processHist.getConfigurationForProcess(shEventProv_.processName(),processConfiguration);
     auto processPSet = edm::pset::Registry::instance()->getMapped(processConfiguration.parameterSetID());
     if(!processPSet) throw cms::Exception("ProvError")<<" warning run "<<run.id()<<" has no config"<<std::endl;
     shEventProv_.setGlobalConfig(processPSet->dump());
     
     std::string processDump = processPSet->dump();
     unsigned char result[MD5_DIGEST_LENGTH];
     MD5((unsigned char*) processDump.c_str(), processDump.size(), result);
     std::ostringstream md5ss;
     md5ss<<std::hex<<std::setfill('0');
     for(long long c: result){
       md5ss<<std::setw(2)<<(long long)c;
     }
     shEventProv_.setMD5SumStr(md5ss.str());
     event_->setConfigMD5SumStr(md5ss.str());
     TList* userInfo = tree_->GetUserInfo();
     userInfo->Add(new SHEventProvenance(shEventProv_));
  }
}
