#include "SHarper/SHNtupliser/interface/HackedFuncs.h"

#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include <cstdlib>

//this is ripped from HcalDetId. It is not my function. If you think this is bad, you should look at HcalDetId::hashed_index();
 bool HackedFuncs::validHcalDetId( int sd,
                        int             ies,
                        int             ip,
                        int             dp      ) 
 {
    const int ie ( abs( ies ) ) ;
 
    return ( ( ip >=  1         ) &&
             ( ip <= 72         ) &&
             ( dp >=  1         ) &&
             ( ie >=  1         ) &&
             ( ( ( sd == HcalBarrel ) &&
                 ( ( ( ie <= 14         ) &&
                     ( dp ==  1         )    ) ||
                   ( ( ( ie == 15 ) || ( ie == 16 ) ) && 
                     ( dp <= 2          )                ) ) ) ||
               (  ( sd == HcalEndcap ) &&
                  ( ( ( ie == 16 ) &&
                      ( dp ==  3 )          ) ||
                    ( ( ie == 17 ) &&
                      ( dp ==  1 )          ) ||
                    ( ( ie >= 18 ) &&
                      ( ie <= 20 ) &&
                      ( dp <=  2 )          ) ||
                    ( ( ie >= 21 ) &&
                      ( ie <= 26 ) &&
                      ( dp <=  2 ) &&
                      ( ip%2 == 1 )         ) ||
                    ( ( ie >= 27 ) &&
                      ( ie <= 28 ) &&
                      ( dp <=  3 ) &&
                      ( ip%2 == 1 )         ) ||
                    ( ( ie == 29 ) &&
                      ( dp <=  2 ) &&
                      ( ip%2 == 1 )         )          )      ) ||
               (  ( sd == HcalOuter ) &&
                  ( ie <= 15 ) &&
                  ( dp ==  4 )           ) ||
               (  ( sd == HcalForward ) &&
                  ( dp <=  2 )          &&
                  ( ( ( ie >= 29 ) &&
                      ( ie <= 39 ) &&
                      ( ip%2 == 1 )    ) ||
                    ( ( ie >= 40 ) &&
                      ( ie <= 41 ) &&
                      ( ip%4 == 3 )         )  ) ) ) ) ;
}
