#!/usr/bin/env python

import os
import optparse
import math

def getCrossSec(dataset):
    if dataset.find("QCD_Pt-30to50")!=-1: return 161500000.
    if dataset.find("QCD_Pt-50to80")!=-1: return 22110000.
    if dataset.find("QCD_Pt-80to120")!=-1: return 3000114.3
    if dataset.find("QCD_Pt-120to170")!=-1: return 493200.
    if dataset.find("QCD_Pt-170to300")!=-1: return 120300.
    if dataset.find("QCD_Pt-300to470")!=-1: return 7475.
    if dataset.find("QCD_Pt-470to600")!=-1: return 587.1
    if dataset.find("QCD_Pt-600to800")!=-1: return 167.
    if dataset.find("QCD_Pt-800to1000")!=-1: return 28.25
    if dataset.find("QCD_Pt-1000to1400")!=-1: return 8.195
    if dataset.find("QCD_Pt-1400to1800")!=-1: return 0.7346
    if dataset.find("QCD_Pt-1800")!=-1: return 0.1091
    if dataset.find("DYToEE_Tune4C")!=-1: return 6870.
    if dataset.find("DYToMuMu_Tune4C")!=-1: return 6960.
    if dataset.find("WToENu_Tune4C")!=-1: return 16000.
    if dataset.find("WToMuNu_Tune4C")!=-1: return 16100.
    return 0


lumi=1.4E34
totRate=0.
totRateErr2=0.
for line in open("emuTrig"):
    fields = line.split(" ")
    dataset=fields[0]
    nrTot=int(fields[3])
    nrPass=int(fields[6])
 #   print dataset,nrTot,nrPass

    crossSec=getCrossSec(dataset)
#    print crossSec

    weight = crossSec*1E-36*lumi/nrTot

    rate = weight*nrPass
    rateErr2 = weight*weight*nrPass

    print dataset,rate,"+/-",rateErr2**(0.5) 

    totRate+=rate
    totRateErr2+=rateErr2

print "tot rate :",totRate,"+/-",totRateErr2**(0.5) 
