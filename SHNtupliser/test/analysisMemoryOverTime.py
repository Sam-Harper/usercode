#!/usr/bin/env python

def processData(content,memData,eventNr):
    active=False
    for line in content:
        line=line.rstrip()
        
     #   if len(line.split())>1 and line.split()[1]=="total": active=True
        if active and line.find("---------")!=-1: return
        if line.find("Flat profile (self")!=-1: active=True

       # print active

        if active:
            data=line.split()[0:4]
            for name in line.split()[3:-1]:
                data[3]+=" "+name
            if len(data)==4:
                data[1]=data[1].replace("'","")
                data[2]=data[2].replace("'","")
            
                percentTot=data[0]
                tot=data[1]
                calls=data[2]
                name=data[3]
            
                if tot=="profile" or tot=="total": continue

                if name not in memData:
                    memData[name]=[]
                          
                memData[name].append([eventNr,tot])
                
def getint(filename):
    return int(filename.split(".")[1])


def makeGraph(dataPoints):
    from ROOT import TGraph
    import numpy as np
    xPoints=[]
    yPoints=[]
    for pointPair in dataPoints:
        try:
            xPoints.append(int(pointPair[0]))
            yPoints.append(int(pointPair[1]))
        except:
            print "error ",pointPair
    graph =TGraph(len(xPoints),np.array(xPoints,np.int32),np.array(yPoints,np.int32))
    return graph

import os
import argparse

parser = argparse.ArgumentParser(description='analysers all the igreports')
parser.add_argument('igReports',help='igReports',nargs="+")
args = parser.parse_args()

memData={}

args.igReports.sort(key=getint)
 
for filename in args.igReports:
    print filename
    with open(filename) as f:
        content =f.readlines()
        eventNr=filename.split(".")[1]
        processData(content,memData,eventNr)
    

#print memData
from ROOT import gROOT, TFile, TF1, TCanvas
rootFile=TFile("test.root","RECREATE")
graphNr=0

c1=TCanvas("c1","c1",900,600)

for key in memData.keys():
    print key
    
    graph=makeGraph(memData[key])
    graph.SetName("graph"+str(graphNr))
    graph.SetTitle(key)
    graph2=makeGraph(memData[key])
    fit=TF1("pol1","pol1")
    graph2.Fit(fit)
    graph.SetMarkerStyle(8)
    graph.GetXaxis().SetTitle("event nr")
    #graph.GetYaxis().SetTitle("nr bytes live")
    graph2.Draw("AP")
  #  c1.Print("plots/graph"+str(graphNr)+".gif")
    if fit.GetParameter(1)>0.01:
        print graphNr,key
        rootFile.WriteTObject(graph,"graph"+str(graphNr))
        graphNr+=1
    

rootFile.Write()

#from ROOT import gROOT, TCanvas, TF1
