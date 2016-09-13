#ifndef TRIGMENUDATAMGR_HH
#define TRIGMENUDATAMGR_HH

#include "SHarper/SHNtupliser/interface/SHTrigMenuData.hh"

#include <string>
#include <unordered_map>
#include <unordered_set>

class TrigMenuDataMgr {
  struct Key {
    std::string menuName;
    std::string procName;
    Key(const SHTrigMenuData& rhs):menuName(rhs.menuName()),procName(rhs.processName()){}
    Key(const std::string& iMenuName,const std::string& iProcName):menuName(iMenuName),procName(iProcName){}
    bool operator==(const Key& rhs)const{return menuName==rhs.menuName && procName==rhs.procName;}
    bool operator!=(const Key& rhs)const{return !(*this==rhs);}
    bool operator<(const Key& rhs)const{
      if(menuName<rhs.menuName) return true;
      else if(menuName>rhs.menuName) return false;
      else return procName<rhs.procName;
    }
  };
    
  struct Hasher{
    std::size_t operator()(const Key& obj) const {
      std::size_t hash1 = std::hash<std::string>()(obj.menuName);
      std::size_t hash2 = std::hash<std::string>()(obj.procName);
      return hash1 ^ (hash2 << 1); // or use boost::hash_combine
    }
  };
  
private:
  std::unordered_map<Key,SHTrigMenuData,Hasher> menuData_;
  const SHTrigMenuData dummyData_;

public:
  
  const SHTrigMenuData& get(const std::string& menuName,const std::string& procName)const;
  bool add(const SHTrigMenuData& data){return menuData_.insert({data,data}).second;}
  void clear(){menuData_.clear();}
  bool hasMenu(const std::string& menuName,const std::string& procName)const;
  


  void read(TTree* tree);
  void write(TTree* tree)const;

private:
  std::unordered_set<Key,Hasher> getStoredKeys_(TTree* tree)const;
};

#endif
