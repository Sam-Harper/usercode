
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CondFormats/DataRecord/interface/GBRDWrapperRcd.h"
#include "CondFormats/EgammaObjects/interface/GBRForestD.h"

#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class DumpRegGBRForest : public edm::EDAnalyzer { 


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
  
  virtual void beginJob()override{}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup)override{}
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  virtual void endJob()override{}
  
private:
  std::vector<std::string> names_;
  bool written_;
};


DumpRegGBRForest::DumpRegGBRForest(const edm::ParameterSet& iPara):
  names_(iPara.getParameter<std::vector<std::string>>("names")),
  written_(false)
{


}

void DumpRegGBRForest::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  if(!written_){
    edm::Service<TFileService> fs;
    edm::ESHandle<GBRForestD> gbrHandle;
    for(const auto& name : names_){
      iSetup.get<GBRDWrapperRcd>().get(name,gbrHandle);
      fs->file().WriteObject(gbrHandle.product(),name.c_str());
    }
    written_=true;
  }
}


DEFINE_FWK_MODULE(DumpRegGBRForest);
