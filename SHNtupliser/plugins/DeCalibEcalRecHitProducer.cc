#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"

class DeCalibEcalRecHitProducer : public edm::EDProducer {
private:
  edm::InputTag inputEBRecHitTag_;
  edm::InputTag inputEERecHitTag_;
  bool doLaser_;
  bool doInterCalib_;
  bool doADCToGeV_;
  bool deCalib_;
  double ebLaserMin_;
  double ebLaserMax_;
  double eeLaserMin_;
  double eeLaserMax_;
public:
  explicit DeCalibEcalRecHitProducer(const edm::ParameterSet & iConfig);
  ~DeCalibEcalRecHitProducer(){}
  void produce(edm::Event & iEvent, const edm::EventSetup& iSetup);
  
  void makeNewRecHitCollection(const edm::Handle<EcalRecHitCollection>& recHitHandle,edm::ESHandle<EcalLaserDbService>& laserHandle,const EcalIntercalibConstantMap& interCalibMap,float adcToGeVCorr,const edm::Timestamp& time,std::unique_ptr<EcalRecHitCollection>& newRecHitCollection);
  float getLaserCorr(const DetId& id,const edm::ESHandle<EcalLaserDbService>& laserHandle,const edm::Timestamp& time)const;
};

DeCalibEcalRecHitProducer::DeCalibEcalRecHitProducer(const edm::ParameterSet & iConfig)
{
  inputEBRecHitTag_=iConfig.getParameter<edm::InputTag>("inputEBRecHitTag");
  inputEERecHitTag_=iConfig.getParameter<edm::InputTag>("inputEERecHitTag");
  doLaser_=iConfig.getParameter<bool>("doLaser");
  doInterCalib_=iConfig.getParameter<bool>("doInterCalib");
  doADCToGeV_=iConfig.getParameter<bool>("doADCToGeV");
  deCalib_=iConfig.getParameter<bool>("deCalib");
  ebLaserMin_=iConfig.getParameter<double>("ebLaserMin");
  ebLaserMax_=iConfig.getParameter<double>("ebLaserMax");
  eeLaserMin_=iConfig.getParameter<double>("eeLaserMin");
  eeLaserMax_=iConfig.getParameter<double>("eeLaserMax");

  produces<EcalRecHitCollection>("EcalRecHitsEB");
  produces<EcalRecHitCollection>("EcalRecHitsEE");
}

void DeCalibEcalRecHitProducer::produce(edm::Event & iEvent, const edm::EventSetup& iSetup)
{
  edm::Handle<EcalRecHitCollection> ebRecHitHandle;
  iEvent.getByLabel(inputEBRecHitTag_,ebRecHitHandle);

  edm::Handle<EcalRecHitCollection> eeRecHitHandle;
  iEvent.getByLabel(inputEERecHitTag_,eeRecHitHandle);

  edm::ESHandle<EcalLaserDbService> laserHandle;
  iSetup.get<EcalLaserDbRecord>().get(laserHandle);
  edm::ESHandle<EcalIntercalibConstants> interCalibHandle;
  iSetup.get<EcalIntercalibConstantsRcd>().get(interCalibHandle);
  const EcalIntercalibConstantMap& interCalibMap = interCalibHandle->getMap(); 

  edm::ESHandle<EcalADCToGeVConstant> adcToGeVHandle;
  iSetup.get<EcalADCToGeVConstantRcd>().get(adcToGeVHandle);

  auto newEBHits = std::make_unique<EcalRecHitCollection>();
  auto newEEHits = std::make_unique<EcalRecHitCollection>();  
  
  makeNewRecHitCollection(ebRecHitHandle,laserHandle,interCalibMap,adcToGeVHandle->getEBValue(),iEvent.time(),newEBHits);
  makeNewRecHitCollection(eeRecHitHandle,laserHandle,interCalibMap,adcToGeVHandle->getEEValue(),iEvent.time(),newEEHits);

  iEvent.put(std::move(newEBHits),"EcalRecHitsEB");  
  iEvent.put(std::move(newEEHits),"EcalRecHitsEE");
}

void DeCalibEcalRecHitProducer::makeNewRecHitCollection(const edm::Handle<EcalRecHitCollection>& recHitHandle,edm::ESHandle<EcalLaserDbService>& laserHandle,const EcalIntercalibConstantMap& interCalibMap,float adcToGeVCorr,const edm::Timestamp& time,std::unique_ptr<EcalRecHitCollection>& newRecHitCollection)
{
  for(EcalRecHitCollection::const_iterator recHit = recHitHandle->begin(); recHit != recHitHandle->end(); ++recHit) {
    DetId id = recHit->id();
    float laserCalib = getLaserCorr(id,laserHandle,time);
    float interCalib = 1.;
    if(doInterCalib_){
      EcalIntercalibConstantMap::const_iterator interCalibIt = interCalibMap.find(id);
      interCalib = interCalibIt==interCalibMap.end() ? 1 : *interCalibIt;
    }
    if(!doADCToGeV_) adcToGeVCorr=1.;
    
    float newEnergy = recHit->energy();
    if(deCalib_) newEnergy/=(laserCalib*interCalib*adcToGeVCorr);
    else newEnergy*=laserCalib*interCalib*adcToGeVCorr;

    EcalRecHit newRecHit(*recHit);
    newRecHit.setEnergy(newEnergy);
    newRecHitCollection->push_back(newRecHit);
  }

}

float DeCalibEcalRecHitProducer::getLaserCorr(const DetId& id,const edm::ESHandle<EcalLaserDbService>& laserHandle,const edm::Timestamp& time)const
{
  float laserCalib = doLaser_ ? laserHandle->getLaserCorrection(id,time) : 1;
  if(id.subdetId()==EcalBarrel){
    if(laserCalib<ebLaserMin_ || laserCalib>ebLaserMax_) laserCalib =1;
  }else if(id.subdetId()==EcalEndcap){
    if(laserCalib<eeLaserMin_ || laserCalib>eeLaserMax_) laserCalib =1;    
  }else{
    std::cout <<"DeCalibEcalRecHitProducer::getLaserCorr : Warning "<<id.subdetId()<<" is not barrel or endcap"<<std::endl;
  }
  return laserCalib;
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(DeCalibEcalRecHitProducer);
