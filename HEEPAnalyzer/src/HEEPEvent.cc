#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"


float heep::Event::getEcalRecHitCalibConst(int rawDetId)const
{
  DetId id(rawDetId);
  if(id.det()!=DetId::Ecal) return 0.;
  if(id.subdetId()!=EcalBarrel && id.subdetId()!=EcalEndcap) return 0.;

  const float laserCorr= handles().ecalLaser->getLaserCorrection(id,timeStamp());
  auto calibIt = handles().ecalInterCalib->getMap().find(id);
  const float interCalib = calibIt!=handles().ecalInterCalib->getMap().end() ? *calibIt : 0;
  return laserCorr * interCalib;

}

float heep::Event::getADCToGeVConst(int rawDetId)const
{
  DetId id(rawDetId);
  if(id.det()!=DetId::Ecal) return 0.;
  
  if(id.subdetId()==EcalBarrel) return handles().ecalADCToGeV->getEBValue();
  else if(id.subdetId()==EcalEndcap) return handles().ecalADCToGeV->getEEValue();
  else return 0.;
}
