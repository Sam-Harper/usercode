from ROOT import TFile, TCanvas, TH1F, TLegend, kBlack, kRed, kBlue, kGreen, kYellow, gStyle

#fName = ["EfficiencyDRnew_allL1Seeds.root","EfficiencyDRnew_opt4.root"]#,"EfficiencyDRnew_noEG26.root","EfficiencyDRnew_noEG28.root"]
fName = ["EfficiencyDRnew_allL1Seeds.root","EfficiencyDRnew_noEG26.root"]#,"EfficiencyDRnew_noEG28.root"]

fIn = dict()
#style  = [24,25,26,32]
style  = [24,32]
#colors = [kBlack, kRed, kBlue, kGreen]#, kYellow]
colors = [kBlack, kBlue]#, kYellow]

fDen = TFile("EfficiencyDRnew_allL1Seeds.root")
d_EB = fDen.Get("EfficiencyCalculator/den_ele_pt_EB")
d_EE = fDen.Get("EfficiencyCalculator/den_ele_pt_EE")
#d_EB.Rebin()
#d_EE.Rebin()

canvas_EB = TCanvas()
canvas_EB.SetGrid(1)

canvas_EE = TCanvas()
canvas_EE.SetGrid(1)

gStyle.SetOptStat(0)
gStyle.SetLegendTextSize(0.04);
leg_EB = TLegend(0.5,0.5,0.9,0.7)
leg_EB.SetHeader(" ")
leg_EB.SetFillColor(0)
leg_EB.SetBorderSize(0)
leg_EB.SetLineColor(1)
leg_EB.SetLineStyle(1)
leg_EB.SetLineWidth(1)
leg_EB.SetFillStyle(0)

leg_EE = TLegend(0.5,0.5,0.9,0.7)
leg_EE.SetHeader(" ")
leg_EE.SetFillColor(0)
leg_EE.SetBorderSize(0)
leg_EE.SetLineColor(1)
leg_EE.SetLineStyle(1)
leg_EE.SetLineWidth(1)
leg_EE.SetFillStyle(0)

for f,s,c in zip(fName,style,colors):
    fIn[f] = TFile(f)
    n_EB = fIn[f].Get("EfficiencyCalculator/num_ele_pt_passL1seeds_EB")
    n_EE = fIn[f].Get("EfficiencyCalculator/num_ele_pt_passL1seeds_EE")
 
    #n_EB.Rebin()
    #n_EE.Rebin()

    print(f)
    # if f == "Efficiency_allL1Seeds.root":
    #     d_EB = fAll.Get("EfficiencyCaluclator/den_ele_pt_EB")
    #     d_EE = fAll.Get("EfficiencyCaluclator/den_ele_pt_EE")
    
    n_EB.Divide(d_EB)
    n_EE.Divide(d_EE)
    n_EB.GetYaxis().SetRangeUser(0,1)
    n_EE.GetYaxis().SetRangeUser(0,1)
    n_EB.GetXaxis().SetTitle("Gen #it{p}_{T} (GeV)")
    n_EE.GetXaxis().SetTitle("Gen #it{p}_{T} (GeV)")
    n_EB.GetYaxis().SetTitle("Efficiency Barrel")
    n_EE.GetYaxis().SetTitle("Efficiency Endcaps")
    n_EB.SetMarkerStyle(s)
    n_EE.SetMarkerStyle(s)
    n_EB.SetMarkerColor(c)
    n_EE.SetMarkerColor(c)
    

    print("n_EB:", n_EB.GetBinContent(7),"n_EE:", n_EE.GetBinContent(7))
    canvas_EB.cd()
    n_EB.Draw("same,P")
    leg_EB.AddEntry(n_EB,f.split("_")[1].replace(".root",""),"p")
    leg_EB.Draw("same")

    canvas_EE.cd()
    n_EE.Draw("same,P")
    leg_EE.AddEntry(n_EE,f.split("_")[1].replace(".root",""),"p")
    leg_EE.Draw("same")
    
canvas_EB.SaveAs("Efficiencies_noEG26_EB.pdf")
canvas_EE.SaveAs("Efficiencies_noEG26_EE.pdf")

#Now the python3 version of raw_input()
input()
