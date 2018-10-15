
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "TLorentzVector.h"

#include <iostream>

class DiEleGenFilter : public edm::stream::EDFilter<> {

private:

  struct Cuts {
    float minEt_;
    float minEta_;
    float maxEta_;
  public:
    Cuts(const edm::ParameterSet& conf):
      minEt_(conf.getParameter<double>("minEt")),      
      minEta_(conf.getParameter<double>("minEta")),
      maxEta_(conf.getParameter<double>("maxEta")){}
    Cuts(float minEt,float minEta,float maxEta):minEt_(minEt),minEta_(minEta),maxEta_(maxEta){}
    bool operator()(const TLorentzVector& p4)const{
      float eta = p4.Eta();
      return p4.Pt()>=minEt_ && eta>=minEta_ && eta<maxEta_;
    }
  };

  Cuts ele1Cuts_;
  Cuts ele2Cuts_;

  edm::EDGetTokenT<LHEEventProduct> lheToken_;
  
  
public:
  explicit DiEleGenFilter(const edm::ParameterSet& para):
    ele1Cuts_(para.getParameter<edm::ParameterSet>("ele1Cuts")),
    ele2Cuts_(para.getParameter<edm::ParameterSet>("ele2Cuts")),
    lheToken_(consumes<LHEEventProduct>(para.getParameter<edm::InputTag>("lheParticles")))
  {}
    
  ~DiEleGenFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
};

bool DiEleGenFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{
  bool pass=false;

  edm::Handle<LHEEventProduct> lheEventHandle;
  event.getByToken(lheToken_,lheEventHandle);
  if(!lheEventHandle.isValid()) pass=true;
  else{
    std::vector<TLorentzVector> eleP4s;
    const lhef::HEPEUP& hepeup = lheEventHandle->hepeup();
    int nrParts  = hepeup.NUP;
    TVector3 pos(0,0,0); //all at zero 
    for(int partNr=0;partNr<nrParts;partNr++){
      if(std::abs(hepeup.IDUP[partNr])==11){       
	TLorentzVector p4;
	p4.SetXYZT(hepeup.PUP[partNr][0],hepeup.PUP[partNr][1],hepeup.PUP[partNr][2],
		   hepeup.PUP[partNr][3]);
	eleP4s.push_back(p4);
      }
    }
    if(eleP4s.size()>2){
      std::cout <<"warning DiEleGenFilter found "<<eleP4s.size()<<" electrons";
      pass = true;
    }else if(eleP4s.size()==1){
      std::cout <<"warning DiEleGenFilter found one electron, odd";
      pass = false;
    }else if(eleP4s.size()==0){
      pass = false;
    }else{
      if( ( ele1Cuts_(eleP4s[0]) && ele2Cuts_(eleP4s[1]) ) ||
	  ( ele1Cuts_(eleP4s[1]) && ele2Cuts_(eleP4s[0]) ) ) pass = true;
    }
  }
  return pass;
  
}
void DiEleGenFilter::endJob()
{ 
//  edm::Service<TFileService> fs;
//  fs->file().cd();
  //quick and dirty hack as writing ints directly isnt working
//  TTree* tree = new TTree("preFilterEventCountTree","Event count");
//  tree->Branch("nrPass",&nrPass_,"nrPass/I");
 // tree->Branch("nrTot",&nrTot_,"nrTot/I");
 // tree->Fill();
  
}  




DEFINE_FWK_MODULE(DiEleGenFilter);
