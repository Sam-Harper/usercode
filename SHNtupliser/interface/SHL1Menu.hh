#ifndef SHEVENT_SHL1MENU_HH
#define SHEVENT_SHL1MENU_HH

#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

#include "TObject.h"
#include "TBits.h"
#include <vector>
#include <string>


//this class holds information the HLT menu used, including the defination of the bits and L1 names
//a menu name and process name uniquely define the HLT menu
//kinda of a glorified struct but kept as a class for consistency 
//needs to go into a TList hence inherits from TObject
class SHL1Menu : public TObject {
public:
  class Seed {
  private:
    size_t bitNr_;
    std::string name_;
    std::vector<double> preScales_;
    bool masked_;
  public:
    Seed():bitNr_(std::numeric_limits<size_t>::max()),masked_(true){}
    Seed(size_t iBitNr,std::string iName,std::vector<double> iPreScales,bool iMasked):
      bitNr_(iBitNr),name_(std::move(iName)),
      preScales_(std::move(iPreScales)),masked_(iMasked){}

    size_t bitNr()const{return bitNr_;}
    const std::string& name()const{return name_;}
    double preScale(int preScaleColumn)const{
      if(preScaleColumn>=0 && static_cast<size_t>(preScaleColumn)<preScales_.size()){
	return preScales_[preScaleColumn];
      }else return -1;
    }
    bool masked()const{return masked_;}
    bool valid()const{return !name_.empty();}
  };

private:
  std::string menuName_;
  std::vector<Seed> l1Seeds_;

  static const Seed nullSeed_;//! never set

  mutable std::vector<std::pair<std::string,size_t> > l1NamesToBits_; //! a cache to fast look up a L1 name based on bits
 
  using StrComp=std::less<std::string>;
  using KeyComp=TempFuncs::PairSortBy1st<std::string,int,StrComp >;
  

public:
  SHL1Menu(){}
  //SHL1Menu(const SHTrigSummary& trigSum);
  virtual ~SHL1Menu(){}

  bool valid()const{return !menuName_.empty();}
  
  const std::string& menuName()const{return menuName_;}
  const std::vector<Seed>& l1Seeds()const{return l1Seeds_;}
  
  size_t l1BitNr(const std::string& l1Name)const;
  const Seed& getSeed(const std::string& l1Name)const{return getSeed(l1BitNr(l1Name));}
  const Seed& getSeed(const size_t bitNr)const{return bitNr<l1Seeds_.size() ? l1Seeds_[bitNr] : nullSeed_;}
  
  bool operator==(const SHL1Menu& rhs)const;
  bool operator!=(const SHL1Menu& rhs)const{return !(*this==rhs);}
  bool operator<(const SHL1Menu& rhs)const;

  void setMenuName(std::string name){menuName_=std::move(name);}
  void setSeeds(std::vector<Seed> seeds){clearCache();l1Seeds_=std::move(seeds);}
  void clear();
  void clearCache(){l1NamesToBits_.clear();}
  
private:
  void buildPathNameToNrMap_()const;
  bool cacheOutOfDate_()const{return l1NamesToBits_.empty();}

  ClassDef(SHL1Menu,2)
};

#endif
