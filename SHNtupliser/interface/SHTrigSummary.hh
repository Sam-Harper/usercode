#ifndef SHEVENT_SHTRIGSUMMARY_HH
#define SHEVENT_SHTRIGSUMMARY_HH

//a class to contain all information about the triggers firing in the event
//its the second gen version of this call, the orginal was not great...

#include "SHarper/SHNtupliser/interface/SHTrigObj.hh"
#include "SHarper/SHNtupliser/interface/SHTrigResult.hh"
#include "SHarper/SHNtupliser/interface/SHBitsDef.hh"

#include <vector>
#include <string>

class SHTrigSummary {

private:  
  class SHTrigObjComp{
  public:
    bool operator()(const SHTrigObj& lhs,const SHTrigObj& rhs){
      if(lhs.type()!=rhs.type()) return lhs.type()<rhs.type();
      else if(lhs.pt()!=rhs.pt()) return lhs.pt()>rhs.pt(); //sorting by increasing pt inside a type
      else if(lhs.eta()!=rhs.eta()) return lhs.eta()<rhs.eta();
      else return lhs.eta()<rhs.phi();	   
    }
  };

  class SHTrigResultsComp{
  public:
    bool operator()(const SHTrigResult& lhs,const SHTrigResult& rhs){return lhs.bitNr()<rhs.bitNr();}
    bool operator()(const SHTrigResult& lhs,size_t rhs){return lhs.bitNr()<rhs;}
    bool operator()(size_t lhs,const SHTrigResult& rhs){return lhs<rhs.bitNr();}
  };

  //event level quantities
  int preScaleColumn_; 
  std::vector<SHTrigObj> trigObjs_; //to seperate by type or not, hmm
  std::vector<SHTrigResult> trigResults_;
  static const SHTrigResult nullTrigResult_;//! 
  static const SHTrigObj nullTrigObj_;//!

  //menu level quantities
  std::string menuName_; //menu name in confdb
  std::string processName_;
  std::string globalTag_;
  SHBitsDef pathBitsDef_;
  SHBitsDef filterBitsDef_;  

public:
  

  SHTrigSummary():preScaleColumn_(-1){}
  virtual ~SHTrigSummary(){}
 
  const std::vector<SHTrigObj>& trigObjs()const{return trigObjs_;}
  const std::vector<SHTrigResult>& trigResults()const{return trigResults_;}

  const std::string& menuName()const{return menuName_;}
  const std::string& processName()const{return processName_;}
  const std::string& globalTag()const{return globalTag_;}
  int preScaleColumn()const{return preScaleColumn_;}

  //we shall see if these stay here or migrate to an seperate class
  const SHTrigResult& getTrig(const std::string& trigName)const;
  const SHTrigObj& getTrigObj(const std::string& trigName,float eta,float phi,float maxDR=SHTrigObj::kStdMaxDR)const;
  std::vector<const SHTrigObj*> getTrigObjs(const std::string& trigName)const;

  const SHTrigObj& getTrigObj(float eta,float phi,int type,float maxDR=SHTrigObj::kStdMaxDR)const{
    auto objs=getTrigObjs(eta,phi,type,maxDR);return objs.empty() ? nullTrigObj_ : *objs.front();
  }
  //returns a list of objects with dR<maxDR ordered by dR
  std::vector<const SHTrigObj*> getTrigObjs(float eta,float phi,int type,float maxDR=SHTrigObj::kStdMaxDR)const;


  const SHBitsDef& pathBitsDef()const{return pathBitsDef_;}
  const SHBitsDef& filterBitsDef()const{return filterBitsDef_;}

  void clearEvent();
  void clearMenu();
  void clearAll(){clearEvent();clearMenu();}

  void setMenuName(const std::string& name){menuName_=name;}
  void setGlobalTag(const std::string& name){globalTag_=name;}
  void setProcessName(const std::string& name){processName_=name;}
  void setPreScaleColumn(int val){preScaleColumn_=val;}
  void setPathBitsDef(const std::set<std::string>& bitNames){pathBitsDef_.setBitsDef(bitNames);}
  void setFilterBitsDef(const std::set<std::string>& bitNames){filterBitsDef_.setBitsDef(bitNames);}
  void addTrigObj(const SHTrigObj& obj){trigObjs_.push_back(obj);}
  void addTrigResult(const SHTrigResult& result){trigResults_.push_back(result);}
  void sort();

  ClassDef(SHTrigSummary,1)
};



#endif
