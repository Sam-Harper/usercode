#include "SHarper/SHNtupliser/interface/SHCaloHit.hh"
#include <cmath>
ClassImp(SHCaloHit)

SHCaloHit::SHCaloHit(int id,float nrgy):
  nrgy_(nrgy),
  detId_(id),
  time_(0),
  flagBits_(0),
  chi2AndErrs_(0),
  amplitude_(0)
{

}

SHCaloHit::SHCaloHit(int iId,float iNrgy,float iTime,uint32_t iFlagBits,float iChi2,float iNrgyErr,float iTimeErr,float iAmplitude):
  nrgy_(iNrgy),
  detId_(iId),
  time_(iTime),
  flagBits_(iFlagBits),
  chi2AndErrs_(0),
  amplitude_(iAmplitude)
{
  setChi2(iChi2);
  setNrgyErr(iNrgyErr);
  setTimeErr(iTimeErr);
}

bool operator<(int lhs,const SHCaloHit& rhs)
{
  return lhs<rhs.detId_;
}

float SHCaloHit::chi2()const
{
  uint32_t rawChi2 = BitPacker::unpack(chi2AndErrs_,kEcalChi2Offset,kEcalChi2NrBits);
  return static_cast<float>(rawChi2) / static_cast<float>((1<<kEcalChi2NrBits)-1) * kEcalChi2Max;
							  
}

float SHCaloHit::nrgyErr()const
{
  uint32_t rawNrgyErr = BitPacker::unpack(chi2AndErrs_,kEcalNrgyErrOffset,kEcalNrgyErrNrBits);
  //copied from EcalRecHit, no idea why it works
  uint16_t exponent = rawNrgyErr >> 10;
  uint16_t significand = ~(0xE<<9) & rawNrgyErr;
  return (float) significand*std::pow(10,exponent-5);
}

//copied straight from EcalRecHit
float SHCaloHit::timeErr()const
{
  uint32_t timeErrorBits = BitPacker::unpack(chi2AndErrs_,kEcalTimeErrOffset,kEcalTimeErrNrBits); 
  if( (0xFF & timeErrorBits) == 0x00 ) return -1;
  // all bits on --> time error over 5 ns (return large value)
  if( (0xFF & timeErrorBits) == 0xFF )return 10000;
  uint8_t exponent = timeErrorBits>>5;
  uint8_t significand = timeErrorBits & ~(0x7<<5);
  return std::pow(2.,exponent)*significand*kEcalTimeLSB/1000.;
}

void SHCaloHit::setChi2(float iChi2)
{
  iChi2=std::min(iChi2,kEcalChi2Max);
  uint32_t rawChi2 = std::lround(iChi2/kEcalChi2Max * ((1<<kEcalChi2NrBits)-1));
  chi2AndErrs_ = BitPacker::pack(chi2AndErrs_,rawChi2,kEcalChi2Offset,kEcalChi2NrBits);
}


//copied straight from EcalRechit
void SHCaloHit::setNrgyErr(float iNrgyErr)
{
  // set the energy uncertainty
  // (only energy >= 0 will be stored)

  uint32_t rawNrgyErr = 0;
  if (iNrgyErr > 0.001) {
    uint16_t exponent = std::lround(floor(log10(iNrgyErr))) + 3;
    uint16_t significand = std::lround(iNrgyErr/pow(10, exponent - 5));
    // use 13 bits (3 exponent, 10 significand)
    rawNrgyErr = exponent << 10 | significand;
  }
  chi2AndErrs_ = BitPacker::pack(chi2AndErrs_,rawNrgyErr,kEcalNrgyErrOffset,kEcalNrgyErrNrBits);
  
}

//this is a copy of EcalUncalibratedRecHit::setJitterError

void SHCaloHit::setTimeErr(float iTimeErr)
{
  // use 8 bits (3 exp, 5 mant) and store in ps
  // has range of 5 ps - 5000 ps
  // expect input in BX units
  // all bits off --> time reco bailed out 
  // all bits on  --> time error over 5 ns
  uint32_t timeErrBits =0;
  if(iTimeErr >=0 && iTimeErr < 5){
    float timeInLSB = (1000*iTimeErr)/kEcalTimeLSB;
    int timeInLSBLog2 = static_cast<int>(std::log2(timeInLSB));
    int exponentTmp = timeInLSBLog2 - 4;
    uint8_t exponent=0;
    if (exponentTmp>0) exponent = exponentTmp;
    uint8_t significand = (int) ( std::lround( timeInLSB / std::pow(2,exponent) )   );
    timeErrBits = exponent<<5 | significand;
    if( (0xFF & timeErrBits) == 0xFF) timeErrBits = 0xFE;
    if( (0xFF & timeErrBits) == 0x00) timeErrBits = 0x01;
  }else if(iTimeErr >= 5) timeErrBits=(0x1 << kEcalTimeErrNrBits)-1; //setting to max val
  
  chi2AndErrs_ = BitPacker::pack(chi2AndErrs_,timeErrBits,kEcalTimeErrOffset,kEcalTimeErrNrBits);
  
}

