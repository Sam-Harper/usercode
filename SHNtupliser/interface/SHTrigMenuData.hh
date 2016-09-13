#ifndef SHEVENT_SHTRIGMENUDATA_HH
#define SHEVENT_SHTRIGMENUDATA_HH

#include "SHarper/SHNtupliser/interface/SHBitsDef.hh"
#include "TObject.h"

#include <vector>
#include <string>

class SHTrigSummary;

//this class holds information the HLT menu used, including the defination of the bits and L1 names
//a menu name and process name uniquely define the HLT menu
//kinda of a glorified struct but kept as a class for consistency 
//needs to go into a TList hence inherits from TObject
class SHTrigMenuData : public TObject {
  
private:
  std::string menuName_; 
  std::string processName_;
  std::string globalTag_;
  SHBitsDef pathBitsDef_;
  SHBitsDef filterBitsDef_; 
  std::vector<std::string> l1Names_;//l1 names are different as they are non-unique (many empty ones) but correspond to a fixed bit, there will be 128 for S1, 512 for S2

public:
  SHTrigMenuData(){}
  SHTrigMenuData(const SHTrigSummary& trigSum);
  virtual ~SHTrigMenuData(){}

  bool valid()const{return !menuName_.empty() && !processName_.empty();}
  
  const std::string& menuName()const{return menuName_;}
  const std::string& processName()const{return processName_;}
  const std::string& globalTag()const{return globalTag_;}

  const SHBitsDef& pathBitsDef()const{return pathBitsDef_;}
  const SHBitsDef& filterBitsDef()const{return filterBitsDef_;}
  const std::vector<std::string>& l1Names()const{return l1Names_;}


  bool operator==(const SHTrigMenuData& rhs)const;
  bool operator!=(const SHTrigMenuData& rhs)const{return !(*this==rhs);}
  bool operator<(const SHTrigMenuData& rhs)const;

  void setMenuName(const std::string& name){menuName_=name;}
  void setGlobalTag(const std::string& name){globalTag_=name;}
  void setProcessName(const std::string& name){processName_=name;}
  void setPathBitsDef(const std::set<std::string>& bitNames){pathBitsDef_.setBitsDef(bitNames);}
  void setFilterBitsDef(const std::set<std::string>& bitNames){filterBitsDef_.setBitsDef(bitNames);}
  void setL1Names(std::vector<std::string> l1Names){l1Names_=std::move(l1Names);}
  
  void clearCache(){pathBitsDef_.clearCache();filterBitsDef_.clearCache();}

  ClassDef(SHTrigMenuData,1)
};

#endif
