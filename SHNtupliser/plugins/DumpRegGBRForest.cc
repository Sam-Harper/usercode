
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CondFormats/DataRecord/interface/GBRDWrapperRcd.h"
#include "CondFormats/GBRForest/interface/GBRForestD.h"

#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class DumpRegGBRForest : public edm::one::EDAnalyzer<> { 


public:
  explicit DumpRegGBRForest(const edm::ParameterSet& iPara);
  virtual ~DumpRegGBRForest(){}
  
  DumpRegGBRForest(const DumpRegGBRForest& rhs)=delete;
  DumpRegGBRForest& operator=(const DumpRegGBRForest& rhs)=delete;

 private:
  template<typename T>
  void getToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& paraName){
    token = consumes<T>(iPara.getParameter<edm::InputTag>(paraName));
  }
  template<typename T,typename R>
  void getTokens(std::vector<edm::ESGetToken<T,R>>& tokens,const std::vector<std::string>& names){
    for(const auto& name : names){
      tokens.emplace_back(esConsumes<T,R>(edm::ESInputTag("",name)));
    }
  }
  
  virtual void beginJob()override{}
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  virtual void endJob()override{}
  
private:
  std::vector<std::string> names_;
  bool written_;
  std::vector<edm::ESGetToken<GBRForestD,GBRDWrapperRcd>> gbrTokens_;
};


DumpRegGBRForest::DumpRegGBRForest(const edm::ParameterSet& iPara):
  names_(iPara.getParameter<std::vector<std::string>>("names")),
  written_(false)
{
  getTokens(gbrTokens_,names_);

}

void DumpRegGBRForest::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  if(!written_){
    edm::Service<TFileService> fs;
    for(size_t tokenNr=0;tokenNr<gbrTokens_.size();tokenNr++){
      const auto& name = names_[tokenNr];
      auto gbrHandle = iSetup.getHandle(gbrTokens_[tokenNr]);
      fs->file().WriteObject(gbrHandle.product(),name.c_str());
    }
    written_=true;
  }
}


DEFINE_FWK_MODULE(DumpRegGBRForest);
