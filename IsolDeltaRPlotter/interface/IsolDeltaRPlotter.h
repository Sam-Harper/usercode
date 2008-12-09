#ifndef SHarper_IsolDeltaRPlotter_IsolDeltaRPlotter_h
#define SHarper_IsolDeltaRPlotter_IsolDeltaRPlotter_h

// Description: A simple example of how to access electron isolations in 21X
//              It creates and fills a histogram (seperated into barrel/endcap) 
//              for each of the 5 isolation for all electrons in the event

//
// Original Author:  Sam Harper (RAL)
//         Created: Fri Nov 28 2008


#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerFwd.h"

class TH1;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}


class IsolDeltaRPlotter : public edm::EDAnalyzer {

private:
  //the input tags to pick up the electrons, superclusters and calo towers
  edm::InputTag electronsTag_;
  edm::InputTag superClustersEBTag_;
  edm::InputTag superClustersEETag_;
  edm::InputTag caloTowersTag_;

  //the histograms of the deltaR of the calo towers to super cluster
  //we do not own these histograms, this TFileService will
  TH1* eleHcalIsolDeltaREBHist_;
  TH1* eleHcalIsolDeltaREEHist_; 
  TH1* scHcalIsolDeltaREBHist_; //sc  = supercluster
  TH1* scHcalIsolDeltaREEHist_;

  int nrBins_;//the number of bins each histogram has
  double xMin_; //min of histograms
  double xMax_;//max of histograms
  
  double maxDeltaR_; //the max deltaR to fill the histograms with
  double minEtaBarrel_; //the min eta of sc to fill barrel hist
  double maxEtaBarrel_; //the max eta of sc to fill barrel hist
  double minEtaEndcap_; //the min eta of sc to fill endcap hist
  double maxEtaEndcap_; //the max eta of sc to fill endcap hist

  //disabling copy and assignment 
  //this class is in theory copyable but it seems odd to allow it
private:
  IsolDeltaRPlotter(const IsolDeltaRPlotter& rhs){}
  IsolDeltaRPlotter& operator=(const IsolDeltaRPlotter& rhs){return *this;}

public:
  explicit IsolDeltaRPlotter(const edm::ParameterSet& iPara);
  virtual ~IsolDeltaRPlotter(){}
  
  virtual void beginJob(const edm::EventSetup& iSetup);
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void endJob();
  
  static void fillHcalIsolDRHist(TH1* hist,const math::XYZPoint& centre,const CaloTowerCollection& caloTowers,double maxDeltaR=999.);

};

#endif
