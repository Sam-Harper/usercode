
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "TTree.h"

struct HLTPathData {
  std::string pathName;
  int code;
  std::pair<float,float> hlt1Pass;
  std::pair<float,float> hlt2Pass;
  std::pair<float,float> hlt1Not2;
  std::pair<float,float> hlt2Not1;
  std::pair<float,float> hlt1And2;
  
  HLTPathData(const std::string& iPathName,int iCode):
    pathName(iPathName),code(iCode),
    hlt1Pass(0.,0.),hlt2Pass(0.,0.),
    hlt1Not2(0.,0.),hlt2Not1(0.,0.),
    hlt1And2(0.,0.){}

  bool operator<(const HLTPathData& rhs)const{return pathName<rhs.pathName;} 

};

class PathComp {
  bool operator()(const HLTPathData& lhs,const HLTPathData& rhs)const{return lhs.pathName<rhs.pathName;}
  bool operator()(const HLTPathData& lhs,const std::string& rhs)const{return lhs.pathName<rhs;}
  bool operator()(const std::string& lhs,const HLTPathData& rhs)const{return lhs<rhs.pathName;}
};

class TrigRateAna : public edm::EDAnalyzer {

private:

  edm::InputTag prevTrigResultsTag_;
  edm::InputTag newTrigResultsTag_;
  edm::InputTag genEvtInfoTag_;
  int datasetCode_;
  std::vector<HLTPathData> pathCompData_;
  std::pair<float,float> tot_;  
  HLTConfigProvider prevHLTConfig_; 
  HLTConfigProvider newHLTConfig_; 

  std::set<int> lumiSecs_;
  
public:
  explicit TrigRateAna(const edm::ParameterSet& para);
  ~TrigRateAna(){}
  
  virtual void beginJob();
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& setup);
  virtual void analyze(const edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
  static std::pair<float,float> calRate(std::pair<float,float> pass,std::pair<float,float> tot,float countsToWeight);
  static std::pair<float,float> calRate(std::pair<float,float> pass,float countsToWeight);
  static float crossSec(int datasetCode);
  static void fill(float weight,std::pair<float,float>& sumwAndSumW2);
  static bool passTrig(const std::string& pathName,const edm::TriggerResults& trigResults,const edm::TriggerNames& trigNames);
  void printRatesMC();
  void printRatesData();
  static std::pair<float,float> calRateInc(std::pair<float,float> hlt1And2,std::pair<float,float> hlt1Not2,std::pair<float,float> hlt2Not1,float countsToRate);
};

TrigRateAna::TrigRateAna(const edm::ParameterSet& para)

{
  datasetCode_=para.getParameter<int>("datasetCode");
  prevTrigResultsTag_ = para.getParameter<edm::InputTag>("prevTrigResultsTag");
  newTrigResultsTag_ =  para.getParameter<edm::InputTag>("newTrigResultsTag");
  genEvtInfoTag_ = para.getParameter<edm::InputTag>("genEvtInfoTag");
}


void TrigRateAna::beginJob()
{

}
void TrigRateAna::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  prevHLTConfig_.init(run,setup,prevTrigResultsTag_.process(),changed); 
  std::cout <<"prev table name "<<prevHLTConfig_.tableName()<<" process "<<prevTrigResultsTag_.process()<<std::endl;
  
  if(changed && !pathCompData_.empty()){ 
    std::cout <<"TrigRateAna::beginRun warning prev HLT menu has changed, results are undefined "<<std::endl;
  }

  newHLTConfig_.init(run,setup,newTrigResultsTag_.process(),changed); 
  std::cout <<"new table name "<<newHLTConfig_.tableName()<<" process "<<newTrigResultsTag_.process()<<std::endl;
  
  if(changed && !pathCompData_.empty()){ 
    std::cout <<"TrigRateAna::beginRun warning new HLT menu has changed, results are undefined "<<std::endl;
  }
  
  //so the plan is to 
  //make copies of the list of path names of new and old menu so we can sort them
  //then we loop over the prev menu path names and see which are also present in the new menu, adding them accordingly
  //then we repeat for the new menu path names, adding any which are just in that meun
  if(pathCompData_.empty()){ //initialising hlt paths names
    std::vector<std::string> prevPathNames = prevHLTConfig_.triggerNames();
    std::vector<std::string> newPathNames = newHLTConfig_.triggerNames();
    std::sort(prevPathNames.begin(),prevPathNames.end());
    std::sort(newPathNames.begin(),newPathNames.end());
    pathCompData_.reserve(std::max(prevPathNames.size(),newPathNames.size()));
    for(size_t prevPathNr=0;prevPathNr<prevPathNames.size();prevPathNr++){
      if(std::binary_search(newPathNames.begin(),newPathNames.end(),prevPathNames[prevPathNr])){ //both have it
	pathCompData_.push_back(HLTPathData(prevPathNames[prevPathNr],0x3));
      }else{ //just in prev menu
	pathCompData_.push_back(HLTPathData(prevPathNames[prevPathNr],0x1));
      }
    }//end loop of prev menus paths

    for(size_t newPathNr=0;newPathNr<newPathNames.size();newPathNr++){
      if(!std::binary_search(prevPathNames.begin(),prevPathNames.end(),newPathNames[newPathNr])){ //only in new menu
	pathCompData_.push_back(HLTPathData(newPathNames[newPathNr],0x2));
      }// if was in both, already added
    }//end loop of new menus paths

  }//end check that pathCompData was empty
  std::sort(pathCompData_.begin(),pathCompData_.end());
}

void TrigRateAna::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 

  int lumiSec =iEvent.luminosityBlock();
  lumiSecs_.insert(lumiSec);

  //  std::cout <<"here "<<std::endl;
  edm::Handle<edm::TriggerResults> prevTrigResultsHandle;
  iEvent.getByLabel(prevTrigResultsTag_,prevTrigResultsHandle);
  
  edm::Handle<edm::TriggerResults> newTrigResultsHandle;
  iEvent.getByLabel(newTrigResultsTag_,newTrigResultsHandle);
  
  edm::Handle<GenEventInfoProduct> genEvtInfo;
  iEvent.getByLabel(genEvtInfoTag_,genEvtInfo);
  
  float weight = 1;
  if(genEvtInfo.isValid()) weight=genEvtInfo->weight();

  const edm::TriggerNames& prevTrigNames = iEvent.triggerNames(*prevTrigResultsHandle);  
  const edm::TriggerNames& newTrigNames = iEvent.triggerNames(*newTrigResultsHandle); 
  
  for(auto& pathData : pathCompData_ ){
    bool passPrev = passTrig(pathData.pathName,*prevTrigResultsHandle,prevTrigNames);
    bool passNew = passTrig(pathData.pathName,*newTrigResultsHandle,newTrigNames);
    if(passPrev) fill(weight,pathData.hlt1Pass);
    if(passNew) fill(weight,pathData.hlt2Pass);
    if(passPrev && passNew) fill(weight,pathData.hlt1And2);
    if(passPrev && !passNew) fill(weight,pathData.hlt1Not2);
    if(!passPrev && passNew) fill(weight,pathData.hlt2Not1);
    
    
  }
  fill(weight,tot_);
}

void TrigRateAna::fill(float weight,std::pair<float,float>& sumwAndSumW2)
{
  sumwAndSumW2.first+=weight;
  sumwAndSumW2.second+=weight*weight;
}

std::pair<float,float> TrigRateAna::calRate(std::pair<float,float> pass,std::pair<float,float> tot,float countsToWeight)
{ 
  std::pair<float,float> retVal(0.,0.);
  if(tot.first==0) return retVal;
  float nrFail = tot.first-pass.first;
  float nrFailErr2 = tot.second-pass.second;

  float rateErr = sqrt(pass.first*pass.first* nrFailErr2   + pass.second*nrFail*nrFail)/tot.first/tot.first;

  retVal.first=pass.first/tot.first*countsToWeight;
  retVal.second=rateErr*countsToWeight;
  return retVal;
}
std::pair<float,float> TrigRateAna::calRate(std::pair<float,float> pass,float countsToWeight)
{ 
  std::pair<float,float> retVal(0.,0.0);
 
  retVal.first=pass.first*countsToWeight;
  retVal.second=sqrt(pass.second)*countsToWeight;
  return retVal;
}

std::pair<float,float> TrigRateAna::calRateInc(std::pair<float,float> hlt1And2,std::pair<float,float> hlt1Not2,std::pair<float,float> hlt2Not1,float countsToRate)
{
  std::pair<float,float> retVal(0.,0.);


  float totHLT2 = hlt1And2.first+hlt2Not1.first;
  float totHLT1 = hlt1And2.first+hlt1Not2.first;
  
  std::cout <<" totHLT1 "<<totHLT1<<" totHLT2 "<<totHLT2<<std::endl;

  if(totHLT1==0) return retVal;

  float rateInc = totHLT2/totHLT1;
  
  float totHLT1Sq = totHLT1*totHLT1;
  float totDiff =  hlt2Not1.first-hlt1Not2.first;
  float rateErr = sqrt(hlt2Not1.second/totHLT1Sq +
		       hlt1And2.second*totDiff*totDiff/totHLT1Sq/totHLT1Sq +
		       hlt1Not2.second*totHLT2*totHLT2/totHLT1Sq/totHLT1Sq);
  
  retVal.first=rateInc;
  retVal.second=rateErr;
  return retVal;
}

bool TrigRateAna::passTrig(const std::string& pathName,const edm::TriggerResults& trigResults,const edm::TriggerNames& trigNames)
{
  size_t pathIndex = trigNames.triggerIndex(pathName);
  if(pathIndex<trigResults.size()){
    return trigResults.accept(pathIndex);
  }
  return false;
}

void TrigRateAna::endJob()
{ 
  if(datasetCode_==0) printRatesData();
  else printRatesMC();
  
}
void TrigRateAna::printRatesMC()
{
  float countsToRate = crossSec(datasetCode_)*1E-36*7E33;
  // countsToRate=1;
  for(auto& pathData : pathCompData_ ){
    auto rate1 = calRate(pathData.hlt1Pass,tot_,countsToRate);
    auto rate2 = calRate(pathData.hlt2Pass,tot_,countsToRate);
    auto rate1And2 = calRate(pathData.hlt1And2,tot_,countsToRate);
    auto rate1Not2 = calRate(pathData.hlt1Not2,tot_,countsToRate); 
    auto rate2Not1 = calRate(pathData.hlt2Not1,tot_,countsToRate); 
      //    auto rateInc = calRateInc(pathData.hlt1And2,pathData.hlt1Not2,pathData.hlt2Not1,countsToRate);
    
    auto printFunc =[](std::pair<float,float>& data){std::cout <<","<<data.first<<",+/-,"<<data.second;};
    std::cout <<pathData.pathName;
    printFunc(rate1);
    printFunc(rate2);
    printFunc(rate1And2);
    printFunc(rate1Not2);
    printFunc(rate2Not1);
    std::cout<<std::endl;
    

   
  }
  
}  
void TrigRateAna::printRatesData()
{
  float countsToRate = 25/(lumiSecs_.size()*23.3)*7/.237;
  tot_.first=1;
  tot_.second=0;

  for(auto& pathData : pathCompData_ ){
    auto rate1 = calRate(pathData.hlt1Pass,countsToRate);
    auto rate2 = calRate(pathData.hlt2Pass,countsToRate);
    auto rate1And2 = calRate(pathData.hlt1And2,countsToRate);
    auto rate1Not2 = calRate(pathData.hlt1Not2,countsToRate); 
    auto rate2Not1 = calRate(pathData.hlt2Not1,countsToRate); 

    
    auto printFunc =[](std::pair<float,float>& data){std::cout <<","<<data.first<<",+/-,"<<data.second;};
    std::cout <<pathData.pathName;
    printFunc(rate1);
    printFunc(rate2);
    printFunc(rate1And2);
    printFunc(rate1Not2);
    printFunc(rate2Not1);
    std::cout<<std::endl;
    

   
  }
  
} 
float TrigRateAna::crossSec(int datasetCode)
{
  if(datasetCode==601) return 1837410000;
  else if(datasetCode==602) return 140932000;
  else if(datasetCode==603) return 19204300;
  else if(datasetCode==604) return 2762530;
  else if(datasetCode==605) return 471100;
  else if(datasetCode==606) return 117276;
  else if(datasetCode==607) return 7823;
  else if(datasetCode==608) return 648.2;
  else if(datasetCode==609) return 186.9;
  else if(datasetCode==610) return 32.3486;
  else if(datasetCode==611) return 9.4183;
  else if(datasetCode==612) return 0.84265;
  else if(datasetCode==613) return 0.114943;
  else if(datasetCode==614) return 0.00682981;
  else if(datasetCode==615) return 0.000165445;

  else if(datasetCode==101) return 2008.4*3*4.37; //DYToLL
  else if(datasetCode==211) return 20508.9*3; //WJets
  else return 1;
}


DEFINE_FWK_MODULE(TrigRateAna);
