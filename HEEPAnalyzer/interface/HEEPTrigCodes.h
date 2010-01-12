#ifndef SHARPER_HEEPANALZER_HEEPTRIGCODES
#define SHARPER_HEEPANALZER_HEEPTRIGCODES


//author: Sam Harper
//aim: to define the trigger bits we are interested in
//this is a transisent association of bits to filter names (transisent as its only valid for a single job) 
//the associations will change if the filter names change
//very similar to heep::CutCodes

#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <bitset>
#include <algorithm>

//unforunately hard coded limit of 64 bits which needs to be checked when setting it up
//if this becomes a problem, will be replaced by boost::dynamic_bitset
//my appologies for the typedef, it is better this way 

namespace heep {
  class TrigCodes {
    
  public:
    static const int maxNrBits_=64;
    typedef std::bitset<maxNrBits_> TrigBitSet;
    
    class TrigBitSetMap {
      
    private:
      //sorted vector (required to mainually call sort after its been filled
      std::vector<std::pair<std::string,TrigBitSet> > codeDefs_;
      
    public:
      TrigBitSetMap(){}
      ~TrigBitSetMap(){}
      
    public:
      TrigBitSet getCode(const std::string& descript)const; //one string with entries seperated by ":"
      TrigBitSet getCode(const std::vector<std::string>& descripts)const; //each entry has own string
      void getCodeName(TrigBitSet code,std::string& id)const;
      //convenience function, slower but not that slower
      std::string getCodeName(TrigBitSet code)const{std::string id;getCodeName(code,id);return id;}

      //modifiers
      void setCode(const std::string& descript,TrigBitSet code);
      void setCode(const std::string& descript,int bitNr);
      
      //key comp
      static bool keyComp(const std::pair<std::string,TrigBitSet>& lhs,const std::pair<std::string,TrigBitSet>& rhs);
      void sort(){std::sort(codeDefs_.begin(),codeDefs_.end(),keyComp);}
      size_t size()const{return codeDefs_.size();}
      void printCodes();
    };
    
    
  private:
    static TrigBitSetMap trigBitSetMap_;
    
  private:
    TrigCodes(){}//not allowed to instanstiate
    ~TrigCodes(){}
    
  public:
    //this is called once per job
    //if a filter name is not in filterNames, no bit is assigned to that filter and this class can
    //not be used with it
    static void setCodes(std::vector<std::string>& filterNames);
 

    //access allowed via ":" entry seperated string or a vector of strings
    static TrigBitSet getCode(const std::string& descript){return trigBitSetMap_.getCode(descript);}
    static TrigBitSet getCode(const std::vector<std::string>& descripts){return trigBitSetMap_.getCode(descripts);}
    static void getCodeName(TrigBitSet code,std::string& id){return trigBitSetMap_.getCodeName(code,id);}
    static std::string getCodeName(TrigBitSet code){return trigBitSetMap_.getCodeName(code);}
    static int maxNrBits(){return maxNrBits_;}
    static void printCodes(){return trigBitSetMap_.printCodes();}
    
  };
}

#endif
