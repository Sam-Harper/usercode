#ifndef SHEVENT_SHHLTMENU_HH
#define SHEVENT_SHHLTMENU_HH

#include "SHarper/SHNtupliser/interface/SHBitsDef.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "TObject.h"

#include <vector>
#include <string>



//this class holds information the HLT menu used, including the defination of the bits and L1 names
//a menu name and process name uniquely define the HLT menu
//kinda of a glorified struct but kept as a class for consistency 
//needs to go into a TList hence inherits from TObject
class SHHLTMenu : public TObject {
public:
  class Path {
  private:
    size_t pathNr_;
    std::string name_;
    std::vector<double> preScales_;
    std::vector<std::string> l1Seeds_;
    
  public:
    Path():pathNr_(std::numeric_limits<size_t>::max()){}
    Path(size_t iPathNr,std::string iName,std::vector<double> iPreScales,std::vector<std::string> iL1Seeds):
      pathNr_(iPathNr),name_(std::move(iName)),
      preScales_(std::move(iPreScales)),l1Seeds_(std::move(iL1Seeds)){}

    size_t pathNr()const{return pathNr_;}
    const std::string& name()const{return name_;}
    double preScale(int preScaleColumn)const{
      if(preScaleColumn>=0 && static_cast<size_t>(preScaleColumn)<preScales_.size()){
	return preScales_[preScaleColumn];
      }else return -1;
    }
    const std::vector<std::string>& l1Seeds()const{return l1Seeds_;}
    bool valid()const{return !name_.empty();}
    
  };

private:
  std::string menuName_; 
  std::string processName_;
  std::string globalTag_;
  SHBitsDef filterBitsDef_; 
  std::vector<Path> paths_;

  static const Path nullPath_; //! never set

  mutable std::vector<std::pair<std::string,size_t> > pathNameToNrs_; //! a fast cache to quickly look up a pathname

  using StrComp=std::less<std::string>;
  using KeyComp=TempFuncs::PairSortBy1st<std::string,int,StrComp >;
 
public:
  SHHLTMenu(){}
  // SHHLTMenu(const SHTrigSummary& trigSum);
  virtual ~SHHLTMenu(){}

  bool valid()const{return !menuName_.empty() && !processName_.empty();}
  
  const std::string& menuName()const{return menuName_;}
  const std::string& processName()const{return processName_;}
  const std::string& globalTag()const{return globalTag_;}
  //  const SHBitsDef& pathBitsDef()const{return pathBitsDef_;}
  const SHBitsDef& filterBitsDef()const{return filterBitsDef_;}
  size_t pathBitNr(const std::string& pathName)const;
  const std::vector<Path>& paths()const{return paths_;}
  const Path& getPath(const size_t pathNr)const{return pathNr<paths_.size() ? paths_[pathNr] : nullPath_;}
  const Path& getPath(const std::string& pathName)const{return getPath(pathBitNr(pathName));}
  const std::string& pathName(const size_t pathNr)const{return getPath(pathNr).name();}
  const std::vector<std::string>& l1Seeds(const size_t pathNr)const{return getPath(pathNr).l1Seeds();}

  int preScale(const std::string& pathName,int psColumn)const{return preScale(pathBitNr(pathName),psColumn);}
  int preScale(const size_t pathNr,int psColumn)const;

  bool operator==(const SHHLTMenu& rhs)const;
  bool operator!=(const SHHLTMenu& rhs)const{return !(*this==rhs);}
  bool operator<(const SHHLTMenu& rhs)const;

  void setMenuName(const std::string& name){menuName_=name;}
  void setGlobalTag(const std::string& name){globalTag_=name;}
  void setProcessName(const std::string& name){processName_=name;}
  // template <typename T> void setPathBitsDef(const T& bitNames){pathBitsDef_.setBitsDef(bitNames);}
  template <typename T> void setFilterBitsDef(const T& bitNames){filterBitsDef_.setBitsDef(bitNames);}
  void setPaths(std::vector<SHHLTMenu::Path> paths){pathNameToNrs_.clear();paths_=std::move(paths);}
 
  
  void clearCache(){pathNameToNrs_.clear();filterBitsDef_.clearCache();}
private:
  void buildPathNameToNrMap_()const;
  bool cacheOutOfDate_()const{return pathNameToNrs_.empty();}

  ClassDef(SHHLTMenu,2)
};

#endif

