
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"


#include "TTree.h"
#include "TFile.h"
#include "TH2D.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class DumpEcalChanStatus : public edm::one::EDAnalyzer<> { 


public:
  explicit DumpEcalChanStatus(const edm::ParameterSet& iPara);
  virtual ~DumpEcalChanStatus(){}
  
  DumpEcalChanStatus(const DumpEcalChanStatus& rhs)=delete;
  DumpEcalChanStatus& operator=(const DumpEcalChanStatus& rhs)=delete;

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
  std::string name_;
  bool written_;
};


DumpEcalChanStatus::DumpEcalChanStatus(const edm::ParameterSet& iPara):
  name_(iPara.getParameter<std::string>("name")),
  written_(false)
{


}

int getNrBitsSet(int val)
{
  int nrBitsSet=0;
  while(val!=0){
    if((val&0x1)!=0) nrBitsSet++;
    val = val >> 1;
  }
  return nrBitsSet;
}


template<typename DetIdType>
int getDeadCrysCode(const DetIdType& id,const EcalChannelStatus& ecalChanStatus)
{
  //constexpr int deadChanMask = EcalChannelStatusCode::kDeadFE | EcalChannelStatusCode::kDeadVFE | EcalChannelStatusCode::kNoDataNoTP | EcalChannelStatusCode::kNonRespondingIsolated;
  constexpr int deadChanMask = EcalChannelStatusCode::kNoDataNoTP;
  
  int crysCode =0 ;

  //meh I could derive this from iEtaOrXNr/iPhiOrYNr indices but sometimes simpliest is best
  int bitNr=0;
  for(int iEtaOrXNr=-1;iEtaOrXNr<=1;iEtaOrXNr++){
    for(int iPhiOrYNr=-1;iPhiOrYNr<=1;iPhiOrYNr++){
      if(iEtaOrXNr==0 && iPhiOrYNr==0) continue;
      DetIdType currId = id.offsetBy(iEtaOrXNr,iPhiOrYNr);
      if(currId.rawId()!=0){
	int statusCode = ecalChanStatus[currId.rawId()].getEncodedStatusCode();
	int bit = 0x1 << bitNr;
	if((statusCode&deadChanMask)!=0) crysCode |=bit;
      }
      bitNr++;
    }
  }
  return crysCode;
}

void DumpEcalChanStatus::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  if(!written_){
    edm::Service<TFileService> fs;
    fs->file().cd();
    auto deadChanEBHist = new TH2D("deadChanEBHist","",85*2+1,-85.5,85.5,360,0.5,360.5);
    auto deadChanEEPosHist = new TH2D("deadChanEEPosHist","",100,0.5,100.5,100,0.5,100.5);
    auto deadChanEENegHist = new TH2D("deadChanEENegHist","",100,0.5,100.5,100,0.5,100.5);
    deadChanEBHist->SetDirectory(&fs->file());
    deadChanEEPosHist->SetDirectory(&fs->file());
    deadChanEENegHist->SetDirectory(&fs->file());
    edm::ESHandle<EcalChannelStatus> chanStatusHandle;
    iSetup.get<EcalChannelStatusRcd>().get(chanStatusHandle);
    std::unordered_map<int,int> outMap;
    
    auto& chanStatusCont = *chanStatusHandle;

    for(int iEta=-85;iEta<=85;iEta++){
      if(iEta==0) continue;
      for(int iPhi=1;iPhi<=360;iPhi++){
	EBDetId id(iEta,iPhi);
	auto chanStatus = chanStatusCont[id.rawId()].getEncodedStatusCode();
	int nrBitsSet = getNrBitsSet(chanStatus);
	if(nrBitsSet>1) std::cout <<" multiple bits set "<<chanStatus<<std::endl;
	outMap.insert(std::make_pair<int,int>(id.rawId(),static_cast<int>(chanStatus)));
	deadChanEBHist->SetBinContent(iEta+86,iPhi,getDeadCrysCode(id,chanStatusCont));
      }
    }

    for(int ix=0;ix<=100;ix++){
      for(int iy=0;iy<=100;iy++){
	for(int iz=-1;iz<=1;iz+=2){
	  if(EEDetId::validDetId(ix,iy,iz)){
	    EEDetId id(ix,iy,iz);
	    auto chanStatus = chanStatusCont[id.rawId()].getEncodedStatusCode();
	    outMap.insert(std::make_pair<int,int>(id.rawId(),static_cast<int>(chanStatus)));
	    if(iz<0) deadChanEENegHist->SetBinContent(ix,iy,getDeadCrysCode(id,chanStatusCont));
	    else deadChanEEPosHist->SetBinContent(ix,iy,getDeadCrysCode(id,chanStatusCont));
	  }
	}
      }
    }

	  

    fs->file().WriteObject(&outMap,name_.c_str());
    written_=true;
  }
}


DEFINE_FWK_MODULE(DumpEcalChanStatus);
