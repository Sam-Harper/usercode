#ifndef SHARPER_HEEPANALYZER_HEEPCOMCODES
#define SHARPER_HEEPANALYZER_HEEPCOMCODES

//class: heep::ComCodes
//Description: allows association of binary codes to text strings and easy retreival
//

#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <iostream>

namespace heep {
  class ComCodes { 
    
  private:
    std::vector<std::pair<std::string,int> > _codeDefs;
    
  public:
    ComCodes(){} 
    ComCodes(const ComCodes& rhs):_codeDefs(rhs._codeDefs){}
    ~ComCodes(){} 
    
    //accessors
    int getCode(const char *descript)const;
    void getCodeName(int code,std::string& id)const;
    
    //modifiers
    void setCode(const char *descript,int code);
    
    //key comp
    static bool keyComp(const std::pair<std::string,int>& lhs,const std::pair<std::string,int>& rhs);
    void sort(){std::sort(_codeDefs.begin(),_codeDefs.end(),keyComp);}

  };
}
  
#endif
