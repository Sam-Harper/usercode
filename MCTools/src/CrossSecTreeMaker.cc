// -*- C++ -*-
//
// Package:    CrossSecTreeMaker
// Class:      CrossSecTreeMaker
// 
/**\class CrossSecTreeMaker CrossSecTreeMaker.cc SHarper/CrossSecTreeMaker/src/CrossSecTreeMaker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Sam James Harper,40 3-A01,+41227671599,
//         Created:  Mon Jun 18 11:03:33 CEST 2012
// $Id$
//
//


// system include files
#include <memory>


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "FWCore/Framework/interface/Run.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TTree.h"

class CrossSecTreeMaker : public edm::EDAnalyzer {

  TTree* tree_;
  float mass_;
  std::string datasetName_;
  int datasetCode_;
  float crossSec_;
  float crossSecErr_;
  float cmsEnergy_;
  

public:
  explicit CrossSecTreeMaker(const edm::ParameterSet&);
  ~CrossSecTreeMaker();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob(){}
  
  virtual void beginRun(edm::Run const&, edm::EventSetup const&){}
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  

      
};


CrossSecTreeMaker::CrossSecTreeMaker(const edm::ParameterSet& iConfig)

{ 

  mass_ = iConfig.getParameter<double>("mass");
  datasetName_ = iConfig.getParameter<std::string>("datasetName");
  datasetCode_ = iConfig.getParameter<int>("datasetCode");
  cmsEnergy_ = iConfig.getParameter<double>("cmsEnergy");
  

}


CrossSecTreeMaker::~CrossSecTreeMaker()
{
 
}


void CrossSecTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   
}


// ------------ method called once each job just before starting event loop  ------------
void CrossSecTreeMaker::beginJob()
{
  edm::Service<TFileService> fs;
  fs->file().cd();
  tree_= new TTree("crossSecTree","PDF Tree");
  tree_->Branch("crossSec",&crossSec_,"crossSec/F");
  tree_->Branch("crossSec",&crossSecErr_,"crossSecErr/F");
  tree_->Branch("datasetCode",&datasetCode_,"datasetCode/I");
  tree_->Branch("datasetName",&datasetName_);
  tree_->Branch("mass",&mass_);
  tree_->Branch("cmsEnergy",&cmsEnergy_);
}


void CrossSecTreeMaker::endRun(edm::Run const& iRun, edm::EventSetup const&)
{
  edm::Handle< GenRunInfoProduct > genInfoProduct;
  iRun.getByLabel("generator", genInfoProduct );
  crossSec_ = genInfoProduct->internalXSec().value();
  crossSecErr_ = genInfoProduct->internalXSec().error();
  tree_->Fill();
  
}



//define this as a plug-in
DEFINE_FWK_MODULE(CrossSecTreeMaker);
