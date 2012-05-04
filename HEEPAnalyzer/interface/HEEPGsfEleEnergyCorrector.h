#ifndef SHarper_HEEPAnalyzer_HEEPGsfEleEnergyCorrector_h
#define SHarper_HEEPAnalyzer_HEEPGsfEleEnergyCorrector_h

// This class makes a new collection of GsfElectrons with fEt correction removed
// note, there is no way of knowing programtically if the fEt was already applied or removed
// so some user awarenesses of the problem is needed

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class HEEPGsfEleEnergyCorrector : public edm::EDProducer {

private:
  edm::InputTag inputEleTag_;
 

public:
  explicit HEEPGsfEleEnergyCorrector(const edm::ParameterSet& iPara);
  virtual ~HEEPGsfEleEnergyCorrector(){}
  
private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  
public:
  static float fEtCorr(float et,int type);
};

#endif
