
#include <vector>
#include <iostream>
int layer(int info,int hitNr)
{
  int layersInfo = (info >> 4)&0xFF;
  int hitCounter=0;
  for(int bitNr=0;bitNr<8;bitNr++){
    int bit = 0x1 << bitNr;
    if((layersInfo&bit)!=0){
      if(hitCounter==hitNr) return bitNr+1;
      else hitCounter++;
    }
  }
  return -1;
  
}

int nrLayers(int info)
{
  return (info >> 12)&0xF;
}

int nrHits(int info)
{
  int hitCounter=0;
  int layersInfo = (info >> 4)&0xFF;
  for(int bitNr=0;bitNr<8;bitNr++){
    int bit = 0x1 << bitNr;
    if((layersInfo&bit)!=0) hitCounter++;
  }
  return hitCounter;
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
