#ifndef SHARPER_HEEPANALYZER_HEEPCOMCODES
#define SHARPER_HEEPANALYZER_HEEPCOMCODES

//class: heep::ComCodes
//Description: allows association of binary codes to text strings and easy retreival
//Implimentation: 
//    uses a sorted vector to emulate a map which it uses to convert strings to bits and vice versa
//    items in the string are seperated by a ":" token

#include <cstring>
#include <string>
#include <vector>
#include <iostream>

namespace heep {
  class ComCodes { 
    
  private:
    std::vector<std::pair<std::string,int> > _codeDefs; //sorted vector (basically emulates a map but is a lot faster for our purposes), however an explict call to sort is required at the end
    
  public:
    ComCodes(){} 
    ComCodes(const ComCodes& rhs):_codeDefs(rhs._codeDefs){}
    ~ComCodes(){} 
    
    //accessors
    int getCode(const char *descript)const;
    void getCodeName(int code,std::string& id)const;
    std::string getCodeName(int code)const{std::string id;getCodeName(code,id);return id;} //convient but slightly slower version of the function
    
    //modifiers
    void setCode(const char *descript,int code);
    
    //key comp
    static bool keyComp(const std::pair<std::string,int>& lhs,const std::pair<std::string,int>& rhs);
    void sort(){std::sort(_codeDefs.begin(),_codeDefs.end(),keyComp);}

  };
}
  
#endif
