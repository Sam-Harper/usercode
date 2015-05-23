#!/usr/bin/env python 

import os 

#masses=[2000,2500,3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,8000,8500,9000,9500,10000,10500,11000,11500,12000,12500,13000,13500,14000,14500,15000]

masses=[500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000,2100,2200,2300,2400,2500,2600,2700,2800,2900,3000,3100,3200,3300,3400,3500,3600,3700,3800,3900,4000,4100,4200,4300,4400,4500,4600,4700,4800,4900,5000,5100,5200,5300,5400,5500,5600,5700,5800,5900,6000,6100,6200,6300,6400,6500,6600,6700,6800,6900,7000]

#masses=[400,500]
#kMpls=[0.01,0.02,0.03,0.04,0.05,0.06,0.08,0.09,0.1,0.11,0.12]
kMpls=[0.01,0.05,0.1]
#kMpls=[0.01]
comEnergy=13
nrEvents=1000
for kMpl in kMpls:
    for mass in masses:
        minMass=mass-650
        minMassStr=str(minMass)
        if minMass<0:
            minMass=-1
            minMassStr="0"
        
        maxMass=mass+650
        kMplStr = str(kMpl).replace('.','p')
	basename="rsGravDiEleKMpl"+kMplStr+"_M"+str(mass)+"_M"+minMassStr+"to"+str(maxMass)+"_pythia_"+str(comEnergy)+"TeV"
	cmd = "cmsRun rsGravCrossSecCalc_cfg.py mass="+str(mass)+" "+" comEnergy="+str(comEnergy)+" kMpl="+str(kMpl)+" outFile="+basename+".root maxEvents="+str(nrEvents)+" minMass="+str(minMass)+" maxMass="+str(maxMass)+" >& "+basename+".log"


	print cmd
	os.system(cmd)

