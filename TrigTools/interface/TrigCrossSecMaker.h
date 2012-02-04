#ifndef SHARPER_TRIGTOOLS_TRIGCROSSSECMAKER
#define SHARPER_TRIGTOOLS_TRIGCROSSSECMAKER


//this is a simple class which simulates another trigger based on the already calculated trigger object P4s and filters on that

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include <vector>
#include <string>

namespace trigger{
  class TriggerEvent;
}

namespace edm{
  class Event;
  class EventSetup;
  class ParameterSet;
}



class TrigCrossSecMaker : public edm::EDAnalyzer {
private:
  class TrigCrossSecData {
    class LumiSecData {

    private:
      static const float kLumiSecLength;
      int runnr_;
      int lumiSec_;
      float instLumiDelivered_;
      float instLumiRecorded_;
      int nrEntries_; //uncorrected for prescale
      float nrWeights_; //corrected for prescale

    public:
      LumiSecData():runnr_(0),lumiSec_(0),instLumiDelivered_(0.),
		    instLumiRecorded_(0.),nrEntries_(0),nrWeights_(0){}
      LumiSecData(int iRunnr,int iLumiSec,float iInstLumiDelivered,float iInstLumiRecorded):
	runnr_(iRunnr),lumiSec_(iLumiSec),instLumiDelivered_(iInstLumiDelivered),
	instLumiRecorded_(iInstLumiRecorded),nrEntries_(0),nrWeights_(0){}
	//	std::cout <<"runnr "<<runnr_<<" lumi "<<lumiSec_<<" instLumi deliv "<<instLumiDelivered_<<" record "<<instLumiRecorded_<<std::endl;}
      
      float crossSec()const{return instLumiRecorded_!=0. ? nrWeights_/instLumiRecorded_/kLumiSecLength : 0;}
      float crossSecErr()const{return instLumiRecorded_!=0. ? sqrt(nrEntries_)*nrWeights_/instLumiRecorded_/kLumiSecLength : 0;}
      float instLumiDelivered()const{return instLumiDelivered_;}
      float instLumiRecorded()const{return instLumiRecorded_;}
      int nrEntries()const{return nrEntries_;}
      float nrWeights()const{return nrWeights_;}
      
      void addEvent(int weight){nrWeights_+=weight;nrEntries_++;}
   
      bool operator<(const LumiSecData& rhs)const{
	if(runnr_<rhs.runnr_) return true;
	else if(runnr_>rhs.runnr_) return false;
	else if(lumiSec_<rhs.lumiSec_) return true;
	else return false;
      }
    };

  private: 
    std::vector<LumiSecData> lumiSecData_; //sorted vector
    
  public:
    TrigCrossSecData(){}
    TrigCrossSecData(const std::string& filename){addLumiSecData(filename);}

    void addLumiSecData(const std::string& filename);
    void fill(int runnr,int lumiSec,int nrEvents);
    
    size_t nrLumiSecs(){return lumiSecData_.size();}
    const LumiSecData& lumiSec(size_t lumiSecIndex){return lumiSecData_[lumiSecIndex];}
  };
    
 

private:
   
  edm::InputTag trigResultsTag_;
  std::string pathName_;
  std::string selectingPathName_; //as this is designed for when we re-run the trigger we need the orginal path which selected these events for prescales
  std::string outputFilename_;
  HLTConfigProvider hltConfig_; //to translate path names to filter names
 
  TrigCrossSecData trigCrossSecData_;

public:
  explicit TrigCrossSecMaker(const edm::ParameterSet& iPara);
  ~TrigCrossSecMaker(){}
  
 private:
  virtual void beginJob(){}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob();
  
  

};

#endif
