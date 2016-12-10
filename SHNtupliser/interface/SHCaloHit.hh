#ifndef SHCALOHIT
#define SHCALOHIT

#include "SHarper/SHNtupliser/interface/BitPacker.hh"

#include "TObject.h"

#include <iostream>
#include "stdint.h"

class SHCaloHit : public TObject {

 private:
  float nrgy_;
  int detId_;
  float time_;
  uint32_t flagBits_; 
  uint32_t chi2AndErrs_;
  float amplitude_;

  static constexpr float kEcalChi2Max=64.;
  static constexpr uint32_t kEcalChi2Offset=0;
  static constexpr uint32_t kEcalChi2NrBits=7;
  static constexpr uint32_t kEcalNrgyErrOffset=kEcalChi2Offset+kEcalChi2NrBits;
  static constexpr uint32_t kEcalNrgyErrNrBits=13;
  static constexpr uint32_t kEcalTimeErrOffset=kEcalNrgyErrOffset+kEcalNrgyErrNrBits;
  static constexpr uint32_t kEcalTimeErrNrBits=8;
  static constexpr float kEcalTimeLSB = 1.26008;

 public:
  SHCaloHit():nrgy_(0.),detId_(0),time_(0),flagBits_(0),chi2AndErrs_(0),amplitude_(0){}
  SHCaloHit(int iId,float iNrgy,float iTime,uint32_t iFlagBits,
	    float iChi2,float iNrgyErr,float iTimeErr,float iAmplitude);
  SHCaloHit(int iId,float iNrgy);
  ~SHCaloHit(){}
  

  float nrgy()const{return nrgy_;}
  int detId()const{return detId_;}
  float time()const{return time_;}
  uint32_t flagBits()const{return flagBits_;}
  float chi2()const;
  float nrgyErr()const;
  float timeErr()const;
  float amplitude()const{return amplitude_;}

  void setNrgy(float iNrgy){nrgy_=iNrgy;}
  void setDetId(int iDetId){detId_=iDetId;} 
  void setTime(float iTime){time_=iTime;}
  void setFlagBits(uint32_t iFlagBits){flagBits_=iFlagBits;}
  void setChi2(float iChi2);
  void setNrgyErr(float iNrgyErr);
  void setTimeErr(float iTimeErr);
  void setAmplitude(float iAmp){amplitude_=iAmp;}

  

  bool operator==(const SHCaloHit& rhs)const{return detId_==rhs.detId_;}
  bool operator<(const SHCaloHit& rhs)const{return detId_<rhs.detId_;}
  bool operator<(int rhs)const{return detId_<rhs;}
  friend bool operator<(int lhs,const SHCaloHit& rhs);

  ClassDef(SHCaloHit,4)

};


//stop gap to remain compatable with old code
typedef SHCaloHit SHEcalHit;
typedef SHCaloHit SHHcalHit;

#endif
