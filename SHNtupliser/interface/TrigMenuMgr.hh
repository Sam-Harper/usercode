#ifndef TRIGMENUMGR_HH
#define TRIGMENUMGR_HH

#include "SHarper/SHNtupliser/interface/SHL1Menu.hh"
#include "SHarper/SHNtupliser/interface/SHHLTMenu.hh"

#include <string>
#include <unordered_map>
#include <unordered_set>

class TrigMenuMgr {
  struct L1Key {
    std::string menuName;
    L1Key(const SHL1Menu& rhs):menuName(rhs.menuName()){}
    L1Key(const std::string& iMenuName):menuName(iMenuName){}
    bool operator==(const L1Key& rhs)const{return menuName==rhs.menuName;}
    bool operator!=(const L1Key& rhs)const{return !(*this==rhs);}
    bool operator<(const L1Key& rhs)const{
      if(menuName<rhs.menuName) return true;
      else return false;
    }
  };
  
  struct HLTKey {
    std::string menuName;
    std::string procName;
    HLTKey(const SHHLTMenu& rhs):menuName(rhs.menuName()),procName(rhs.processName()){}
    HLTKey(const std::string& iMenuName,const std::string& iProcName):menuName(iMenuName),procName(iProcName){}
    bool operator==(const HLTKey& rhs)const{return menuName==rhs.menuName && procName==rhs.procName;}
    bool operator!=(const HLTKey& rhs)const{return !(*this==rhs);}
    bool operator<(const HLTKey& rhs)const{
      if(menuName<rhs.menuName) return true;
      else if(menuName>rhs.menuName) return false;
      else return procName<rhs.procName;
    }
  };

 
  
  struct L1Hasher{
    std::size_t operator()(const L1Key& obj) const {
      std::size_t hash1 = std::hash<std::string>()(obj.menuName);
      return hash1;
    }
  };
  
    
  struct HLTHasher{
    std::size_t operator()(const HLTKey& obj) const {
      std::size_t hash1 = std::hash<std::string>()(obj.menuName);
      std::size_t hash2 = std::hash<std::string>()(obj.procName);
      return hash1 ^ (hash2 << 1); // or use boost::hash_combine
    }
  };
  

  
private:
  std::unordered_map<L1Key,SHL1Menu,L1Hasher> l1Menus_;
  std::unordered_map<HLTKey,SHHLTMenu,HLTHasher> hltMenus_;

  const static SHL1Menu dummyL1Menu_;
  const static SHHLTMenu dummyHLTMenu_;


public:
  
  const SHL1Menu& getL1(const std::string& menuName)const;
  const SHHLTMenu& getHLT(const std::string& menuName,const std::string& procName)const;
  bool add(const SHL1Menu& data){return l1Menus_.insert({data,data}).second;}
  bool add(const SHHLTMenu& data){return hltMenus_.insert({data,data}).second;}
  void clear(){l1Menus_.clear();hltMenus_.clear();}
  bool hasL1Menu(const std::string& menuName)const; 
  bool hasHLTMenu(const std::string& menuName,const std::string& procName)const; 
  
  void read(TTree* tree);
  void write(TTree* tree)const;

private:


  template <typename MenuType,typename Key,typename Hasher> 
  std::unordered_set<Key,Hasher> 
  getStoredKeys_(TTree* tree,const std::string& className)const{
    std::unordered_set<Key,Hasher> keys;
    TList* userInfo = tree->GetUserInfo();
    TIter next(userInfo);
    while(TObject *obj = next()){
      if(obj->ClassName()==className){
	auto shObj = static_cast<MenuType*>(obj);
	keys.insert(*shObj);
      }
    }
    return keys;
  }
  
};

#endif
