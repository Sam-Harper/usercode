
#include <vector>
#include <iostream>
int layer(int info,int hitNr)
{
  int layersInfo = (info >> 4)&0xF;
  int hitCounter=0;
  for(int bitNr=0;bitNr<4;bitNr++){
    int bit = 0x1 << bitNr;
    if((layersInfo&bit)!=0){
      if(hitCounter==hitNr) return bitNr+1;
      else hitCounter++;
    }
  }
  return -1;
  
}

int layerInfoTest(std::vector<int> layers)
{
  int info=0;
  for(size_t hitNr=0;hitNr<layers.size();hitNr++){
    int layerBit = 0x1 << 3 << layers[hitNr] ;    
    info |=layerBit;
  }
  return info;
}
