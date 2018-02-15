#ifndef SHEVENT_SHEVENTPROVENANCE_HH
#define SHEVENT_SHEVENTPROVENANCE_HH

#include "TObject.h"
#include <vector>
#include <string>

class SHEventProvenance : public TObject {
private:
  std::vector<std::string> vidNames_;
  std::string shNtupConfig_;
  std::string globalConfig_;
  std::string processName_;
  std::string md5SumStr_;
  
public:
  SHEventProvenance(){}
  ~SHEventProvenance(){}

  const std::string& shNtupConfig()const{return shNtupConfig_;}
  const std::string& globalConfig()const{return globalConfig_;}
  const std::string& md5SumStr()const{return md5SumStr_;}
  const std::string& processName()const{return processName_;}
  const std::vector<std::string> vidNames()const{return vidNames_;}

  void setVIDNames(std::vector<std::string> vidNames){vidNames_=std::move(vidNames);}
  void setSHNtupConfig(std::string shNtupConfig){shNtupConfig_=std::move(shNtupConfig);}
  void setGlobalConfig(std::string globalConfig){globalConfig_=std::move(globalConfig);}
  void setProcessName(std::string processName){processName_=std::move(processName);}
  void setMD5SumStr(std::string md5SumStr){md5SumStr_=std::move(md5SumStr);}

  ClassDef(SHEventProvenance,1)

};

#endif
