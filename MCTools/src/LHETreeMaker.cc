
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TTree.h"
#include "TParameter.h"

struct EventId {
  
  int runnr;
  int lumiSec;
  int eventnr;
  int datasetCode;
  EventId():runnr(0),lumiSec(0),eventnr(0),datasetCode(-1){}
  EventId(int iRunnr,int iLumiSec,int iEventnr,int iDataset=-1):runnr(iRunnr),lumiSec(iLumiSec),eventnr(iEventnr),datasetCode(iDataset){}

  static std::string contents(){return "runnr/I:lumiSec:eventnr:datasetCode";}
  
  bool operator<(const EventId& rhs){
    if(datasetCode<rhs.datasetCode) return true;
    else if(datasetCode>rhs.datasetCode) return false;
    else { //same dataset
      if(runnr<rhs.runnr) return true;
      else if(runnr>rhs.runnr) return false;
      else{ //same run
	if(lumiSec<rhs.lumiSec) return true;
	else if(lumiSec>rhs.lumiSec) return false;
	else if(eventnr<rhs.eventnr) return true;
	else return false;
      }
    }
  }

  bool operator==(const EventId& rhs){
    return datasetCode==rhs.datasetCode && runnr==rhs.runnr && lumiSec==rhs.lumiSec && eventnr==rhs.eventnr;
  }

  void clear(){runnr=0;lumiSec=0;eventnr=0;datasetCode=0;}

};

struct PDFInfo{
  float id1,id2; //should be ints but useful to have as floats for root reasons
  float x1,pdf1;
  float x2,pdf2;
  float scale;

  PDFInfo():id1(0),id2(0),x1(0),pdf1(0),x2(0),pdf2(0),scale(0){} 
  PDFInfo(const gen::PdfInfo& pdfInfo):id1(pdfInfo.id.first),id2(pdfInfo.id.second),x1(pdfInfo.x.first),pdf1(pdfInfo.xPDF.first),x2(pdfInfo.x.second), pdf2(pdfInfo.xPDF.second),scale(pdfInfo.scalePDF){}

  void fill(const gen::PdfInfo& pdfInfo){
    id1=pdfInfo.id.first;
    id2=pdfInfo.id.second;
    x1=pdfInfo.x.first;
    x2=pdfInfo.x.second;
    pdf1=pdfInfo.xPDF.first; 
    pdf2=pdfInfo.xPDF.second;
    scale=pdfInfo.scalePDF;
  }

  static std::string contents(){return "id1/F:id2:x1:pdf1:x2:pdf2:scale";}
  void clear(){id1=0;id2=0;x1=0;pdf1=0;x2=0;pdf2=0;scale=0;}
};

struct P4Struct {
  float nrgy,et,eta,phi,pt,mass,region;
  void fill(const math::XYZTLorentzVector& p4){
    if(p4.Pt()!=0 && p4.Et()!=0){
      nrgy = p4.E();
      et = p4.Et();
      eta = p4.Eta();
      phi = p4.Phi();
      pt =p4.Pt();
      mass = p4.mag();
      region = std::abs(eta)<1.4442 ? 0 : std::abs(eta)>1.566 && std::abs(eta)<2.5 ? 1 : 10;
    }else clear();
  }
  void fill(const lhef::HEPEUP::FiveVector& p4Array){
    math::XYZTLorentzVector p4;
    p4.SetXYZT(p4Array[0],p4Array[1],p4Array[2],p4Array[3]);
    fill(p4);
  }
  float dR2(const P4Struct& rhs)const{return reco::deltaR2(eta,phi,rhs.eta,rhs.phi);}
  void clear(){nrgy=-999;et=-999;eta=-999;phi=-0;pt=-999;mass=-999;region=-999;}
  static std::string contents(){return "nrgy/F:et:eta:phi:pt:mass:region";}
};

struct LHETreeData {
  EventId evtId;
  int bosonId;
  int ele1Id;
  int ele2Id;
  P4Struct bosonP4;
  P4Struct ele1P4;
  P4Struct ele2P4;
  PDFInfo pdf;
  float crossSec;
  float crossSecErr;
  float comEnergy;
  float diEleMass;

  P4Struct b1P4;
  P4Struct b2P4;
  
  float ele1DR2ToNonEle;
  float ele2DR2ToNonEle;
  float eleDR2ToEle;

  void setBranchAddresses(TTree* tree);
  void createBranches(TTree* tree);
  
  void clear();

};

class LHETreeMaker : public edm::one::EDAnalyzer<> {

private:

  TTree* tree_;
  LHETreeData data_;

  edm::InputTag lheEventTag_;

  int datasetCode_;
  
  
public:
  explicit LHETreeMaker(const edm::ParameterSet& para);
  ~LHETreeMaker(){}
  
  virtual void beginJob();
  virtual void analyze(const edm::Event& event,const edm::EventSetup& setup); 
  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void endJob();
  static bool isEqualToAnyOf(int pid,const std::vector<int>& values);
 
};

LHETreeMaker::LHETreeMaker(const edm::ParameterSet& para)

{
  lheEventTag_=para.getParameter<edm::InputTag>("lheEventTag");
  datasetCode_=para.getParameter<int>("datasetCode");
  
}

void LHETreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree("lheTree","LHE Tree");
  data_.createBranches(tree_);
}

void LHETreeMaker::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  data_.clear();

  data_.evtId.runnr = iEvent.id().run();
  data_.evtId.lumiSec = iEvent.luminosityBlock();
  data_.evtId.eventnr = iEvent.id().event();
  data_.evtId.datasetCode=datasetCode_;
  
  edm::Handle<LHEEventProduct> lheEventHandle;
  iEvent.getByLabel(lheEventTag_,lheEventHandle);

  data_.ele1P4.clear();
  data_.ele2P4.clear();
  data_.b1P4.clear();
  data_.b1P4.clear();
  data_.bosonP4.clear();
  data_.ele1Id=data_.ele2Id=data_.bosonId=0;

  //  std::cout <<"here1  "<<lheEventHandle.product()<<std::endl;
  
  const lhef::HEPEUP& hepeup = lheEventHandle->hepeup();
  //const gen::PdfInfo& pdf = *lheEventHandle->pdf();
  // data_.pdf.fill(pdf);

  //std::cout <<"here "<<std::endl;
  
  const std::vector<int> leptonIds = {11,13,15};
  const std::vector<int> antiLeptonIds = {-11,-13,-15};

  int nrParts  = hepeup.NUP;
  for(int partNr=0;partNr<nrParts;partNr++){
    //   std::cout <<partNr<<" "<<hepeup.IDUP[partNr]<<std::endl;
    if(hepeup.IDUP[partNr]==23){
      data_.bosonId=hepeup.IDUP[partNr];
      data_.bosonP4.fill(hepeup.PUP[partNr]);
    }else if(isEqualToAnyOf(hepeup.IDUP[partNr],leptonIds)){
      data_.ele1Id=hepeup.IDUP[partNr];
      data_.ele1P4.fill(hepeup.PUP[partNr]);
    }else if(isEqualToAnyOf(hepeup.IDUP[partNr],antiLeptonIds)){ 
      data_.ele2Id=hepeup.IDUP[partNr];
      data_.ele2P4.fill(hepeup.PUP[partNr]);
    }else if(hepeup.IDUP[partNr]==5){
      data_.b1P4.fill(hepeup.PUP[partNr]);
    }else if(hepeup.IDUP[partNr]==-5){
      data_.b2P4.fill(hepeup.PUP[partNr]);
    }
      
    
  }
  if(data_.ele1P4.et>=0 && data_.ele2P4.et>=0){
    data_.diEleMass = sqrt(2*data_.ele1P4.et*data_.ele2P4.et*(cosh(data_.ele1P4.eta-data_.ele2P4.eta)-cos(data_.ele1P4.phi-data_.ele2P4.phi)));
    if(data_.b1P4.et>=0 && data_.b2P4.et>=0){
      data_.ele1DR2ToNonEle = std::min(data_.ele1P4.dR2(data_.b1P4),data_.ele1P4.dR2(data_.b2P4));
      data_.ele2DR2ToNonEle = std::min(data_.ele2P4.dR2(data_.b1P4),data_.ele2P4.dR2(data_.b2P4));
    }else{
      data_.ele1DR2ToNonEle=999.;
      data_.ele2DR2ToNonEle=999.;
    }
    data_.eleDR2ToEle=data_.ele1P4.dR2(data_.ele2P4);
    
  }else data_.diEleMass = -999.;
  tree_->Fill();  
}

void LHETreeMaker::beginRun(edm::Run const& iRun, edm::EventSetup const&)
{
 
  edm::Handle< LHERunInfoProduct > lheRunInfoProduct;
  iRun.getByLabel(lheEventTag_, lheRunInfoProduct );

  
  data_.comEnergy=lheRunInfoProduct->heprup().EBMUP.first+lheRunInfoProduct->heprup().EBMUP.second;
  data_.crossSec=lheRunInfoProduct->heprup().XSECUP[0];
  data_.crossSecErr=lheRunInfoProduct->heprup().XERRUP[0];
  

  
}


void LHETreeMaker::endRun(edm::Run const& iRun, edm::EventSetup const&)
{
  TParameter<float>* comEnergy= new TParameter<float>("comEnergy",0);
  TParameter<float>* crossSec=new TParameter<float>("crossSec",0);
  TParameter<float>* crossSecErr=new TParameter<float>("crossSecErr",0);
  edm::Handle< LHERunInfoProduct > lheRunInfoProduct;
  iRun.getByLabel(lheEventTag_, lheRunInfoProduct );

  comEnergy->SetVal(lheRunInfoProduct->heprup().EBMUP.first+lheRunInfoProduct->heprup().EBMUP.second);
  crossSec->SetVal(lheRunInfoProduct->heprup().XSECUP[0]);
  crossSecErr->SetVal(lheRunInfoProduct->heprup().XERRUP[0]);

  //as far as I can tell, the TList now owns them but hey who knows!
  //would it kill the ROOT developers to add a comment saying that Add transfers ownership?
 
  tree_->GetUserInfo()->Add(comEnergy);
  tree_->GetUserInfo()->Add(crossSec);
  tree_->GetUserInfo()->Add(crossSecErr);
}



void LHETreeMaker::endJob()
{ 
 
  
}  


bool LHETreeMaker::isEqualToAnyOf(int pid,const std::vector<int>& values)
{
  for(auto& val : values){
    if(pid==val) return true;
  }
  return false;
}



void LHETreeData::setBranchAddresses(TTree* tree)
{
  std::cout <<"LHETreeData::setBranchAddresses not implimented yet "<<std::endl;
}


void LHETreeData::createBranches(TTree* tree)
{
  tree->Branch("evt",&evtId,EventId::contents().c_str());
  tree->Branch("bosonId",&bosonId,"bosonId/I");
  tree->Branch("ele1Id",&ele1Id,"ele1Id/I");
  tree->Branch("ele2Id",&ele2Id,"ele2Id/I");
  tree->Branch("bosonP4",&bosonP4,P4Struct::contents().c_str());
  tree->Branch("ele1P4",&ele1P4,P4Struct::contents().c_str()); 
  tree->Branch("ele2P4",&ele2P4,P4Struct::contents().c_str()); 
  tree->Branch("pdf",&pdf,PDFInfo::contents().c_str());
  tree->Branch("crossSec",&crossSec,"crossSec/F");
  tree->Branch("crossSecErr",&crossSecErr,"crossSecErr/F");
  tree->Branch("comEnergy",&comEnergy,"comEnergy/F");
  tree->Branch("diEleMass",&diEleMass,"diEleMass/F");
  tree->Branch("b1P4",&b1P4,P4Struct::contents().c_str());
  tree->Branch("b2P4",&b2P4,P4Struct::contents().c_str());
  tree->Branch("ele1DR2ToNonEle",&ele1DR2ToNonEle,"ele1DR2ToNonEle/F");
  tree->Branch("ele2DR2ToNonEle",&ele2DR2ToNonEle,"ele2DR2ToNonEle/F");
  tree->Branch("eleDR2ToEle",&eleDR2ToEle,"eleDR2ToEle/F");
}
void LHETreeData::clear()
{
  evtId.clear();
  bosonId=0;
  ele1Id=0;
  ele2Id=0;
  bosonP4.clear();
  ele1P4.clear();
  ele2P4.clear();
  pdf.clear();
  diEleMass = -999;
  //we do not clear cross-sect, cross-sec err or com energy

}

DEFINE_FWK_MODULE(LHETreeMaker);
