#ifndef SHCALOHIT
#define SHCALOHIT

#include "TObject.h"

#include <iostream>
#include "stdint.h"

class SHCaloHit : public TObject {

 private:
  float nrgy_;
  int detId_;
  float time_;
  uint32_t flag_;

 public:
  SHCaloHit():nrgy_(0.),detId_(0),time_(0),flag_(0){}
  SHCaloHit(int id,float nrgy,float time,uint32_t flag);
  SHCaloHit(int id,float nrgy);
  // SHCaloHit(const SHCaloHit & rhs);
  ~SHCaloHit(){}
  
  //  SHCaloHit& operator=(const SHCaloHit& rhs);

 

  float nrgy()const{return nrgy_;}
  int detId()const{return detId_;}
  float time()const{return time_;}
  uint32_t flag()const{return flag_;}
  
  void setNrgy(float iNrgy){nrgy_=iNrgy;}
  void setDetId(int iDetId){detId_=iDetId;} 
  void setTime(float iTime){time_=iTime;}
  void setFlag(uint32_t iFlag){flag_=iFlag;}


  bool operator==(const SHCaloHit& rhs)const{return detId_==rhs.detId_;}
  
  ClassDef(SHCaloHit,2)

};


//stop gap to remain compatable with old code
typedef SHCaloHit SHEcalHit;
typedef SHCaloHit SHHcalHit;

#endif
