#ifndef SHEVENT_SHGAINSWITCHINFO_HH
#define SHEVENT_SHGAINSWITCHINFO_HH

#include "TObject.h"
#include <vector>

class SHGainSwitchInfo {
private:

  bool dupSC_;
  bool hasGSElePho_;
  float metOrg_;
  float metPhiOrg_;
  float metJustEGFix_;
  float metPhiJustEGFix_;

  std::vector<int> gsHitsUnfixed_;
  
public:
  SHGainSwitchInfo():dupSC_(false),hasGSElePho_(false),metOrg_(-999.),metPhiOrg_(0.),metJustEGFix_(-999.),metPhiJustEGFix_(0.){}
  virtual ~SHGainSwitchInfo(){}

  bool dupSC()const{return dupSC_;}
  bool hasGSElePho()const{return hasGSElePho_;}
  float metOrg()const{return metOrg_;}
  float metJustEGFix()const{return metJustEGFix_;}
  const std::vector<int>& gsHitsUnfixed()const{return gsHitsUnfixed_;}

  void setMETOrg(float met,float phi){metOrg_=met;metPhiOrg_=phi;}
  void setMETJustEGFix(float met,float phi){metJustEGFix_=met;metPhiJustEGFix_=phi;}
  void setFixInfo(bool dupSC,bool hasGSElePho,std::vector<int> gsHitsUnfixed);
  
  void clear();
 
  ClassDef(SHGainSwitchInfo,1)
};


#endif
