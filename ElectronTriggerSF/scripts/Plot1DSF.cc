{
	
gStyle->SetErrorX(.5); // somthing strange with Draw("E2")
char str[100];
double GeV=1e3, norm;
gStyle->SetOptStat(0);

TExec *setex1 = new TExec("setex1","gStyle->SetErrorX(0)");
TExec *setex2 = new TExec("setex2","gStyle->SetErrorX(.5)");


TPaveText * clumi = new TPaveText(0.295829,0.776853,0.388191,0.9131668,"NDC");
clumi->SetFillColor(0); clumi->SetLineColor(0);
clumi->SetTextFont(42); clumi->SetTextSize(0.0499401); clumi->SetBorderSize(0);
double maxRatio=1.2, minRatio=0.8;

//TString path="./";
TString path="/eos/user/m/mpitt/PPS_ttbar/ElTrigger";

TString var, var_name, filename; 
TCut cut = "nano_nElectrons==1 &&  nano_nBJets>1   ";
cut = cut && "nano_ElPT[0]>30 && (nano_ElID[0] & (1 << 5)>0) && abs(nano_ElEta[0])<2.1 && nano_ElMatch[0]";
TString trigger_list;
TString label1, label2, label3, label4;
int nbins; float xmin, xmax;
TString infilemc = "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root";
TString infiledata = "Run2017.root";
bool fitme = false; TString legText = "e#mu+2b selection";

//var = "nano_ElPT[0]"; var_name = "Electron p_{T} [GeV]"; nbins=40; xmin=30; xmax=130; filename="pt";
//var = "nano_ElEta[0]"; var_name = "Electron #eta"; nbins=44; xmin=-2.1; xmax=2.1; filename="eta";

cut = cut && "nano_nJets>3"; legText="e#mu+2bj+2lj selection";
//var = "nano_nJets"; var_name = "jet multiplicity"; nbins=6; xmin=3.5; xmax=9.5; filename="jetn";
var = "(nano_HT+nano_ElPT[0])/1e3"; var_name = "#Sigmap_{T}^{jet}+p_{T}^{el} [TeV]"; nbins=25; xmin=0.075; xmax=1.075; filename="ht";


trigger_list = "HLT_Ele35_WPTight_Gsf || HLT_Ele28_eta2p1_WPTight_Gsf_HT150 || HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned";
//trigger_list = "HLT_Ele28_eta2p1_WPTight_Gsf_HT150";


xlogplot= false;

// define trees and histograms
TChain * t_mc = new TChain("Events"); t_mc->Add(path+"/"+infilemc);
TChain * t_data = new TChain("Events"); t_data->Add(path+"/"+infiledata);

cout << "File "<<infilemc<<", has "<<t_mc->GetEntries()<<" entries"<<endl;
cout << "File "<<infiledata<<", has "<<t_data->GetEntries()<<" entries"<<endl;

TProfile * hmc = new TProfile("hmc",";"+var_name+";Efficiency",nbins,xmin,xmax,-0.5,1.5);
TProfile * hd = new TProfile("hd","",nbins,xmin,xmax,-0.5,1.5);
TH1F * hratio = new TH1F("hratio","",nbins,xmin,xmax);

// plotting efficiency curves:

t_mc->Draw(Form("%s : %s>>hmc",trigger_list.Data(),var.Data()), cut );
t_data->Draw(Form("%s : %s>>hd",trigger_list.Data(),var.Data()), cut );


TCanvas* c1 = new TCanvas("c1","c1",0,50,600,600);

c1->SetTicks(1,1);
c1->SetGrid(1,1);
c1->SetBottomMargin(0.0);
c1->SetLeftMargin(0.0);
c1->SetTopMargin(0);
c1->SetRightMargin(0.00);

TPad* padhigh = new TPad("padhigh","padhigh",0,0.2,1,1);
    padhigh->SetRightMargin(0.05);
    padhigh->SetLeftMargin(0.12);
    padhigh->SetTopMargin(0.06);
    padhigh->SetBottomMargin(0.01);
	padhigh->Draw();
    padhigh->cd();
    padhigh->SetGrid(1,1); padhigh->SetTicks(0,1);

TF1 * fmc = new TF1("fmc","[0]*(1+TMath::Erf((x-[1])/[2]))",0,200);
TF1 * fd = new TF1("fd","[0]*(1+TMath::Erf((x-[1])/[2]))",0,200);
TF1 * fr = new TF1("fr","[0]*(1+TMath::Erf((x-[1])/[2]))",0,200);

hmc->SetMarkerStyle(20);
hmc->SetMarkerColor(kRed);
hmc->SetLineColor(kRed);
fmc->SetLineColor(kRed);
hmc->SetLineWidth(2);

hd->SetMarkerStyle(21);
hd->SetMarkerColor(kBlue);
hd->SetLineColor(kBlue);
fd->SetLineColor(kBlue);
hd->SetLineWidth(2);

// Fits
if(fitme){
hmc->Fit(fmc);
hd->Fit(fd);
}

//clumi->AddText("TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8");

hmc->GetYaxis()->SetRangeUser(0,1.2);
hmc->Draw();
hd->Draw("same");


//clumi->Draw();

	TLatex * text_cms = new TLatex();
	text_cms->SetNDC(true);
    text_cms->SetTextFont(42);
    text_cms->SetTextSize(0.045);
    text_cms->SetTextAlign(12)	;
	text_cms->DrawLatex(0.14,0.97,"#bf{CMS} #it{Internal}");
    text_cms->SetTextSize(0.040);
    text_cms->DrawLatex(0.14,0.825,legText);	
    text_cms->SetTextSize(0.040);
	text_cms->SetTextAlign(kHAlignRight+kVAlignCenter);
	text_cms->DrawLatex(0.95,0.97,Form("#scale[0.9]{%3.1f fb^{-1} (13 TeV)}",41.5));
	
TLegend* lg1 = new TLegend(0.70,0.05,0.90,0.20,"");
lg1->AddEntry(hmc,"MC","LP");
lg1->AddEntry(hd,"2017 data","LP");
    lg1->SetTextFont(42); lg1->SetTextSize(0.033);
    lg1->SetBorderSize(0);
    lg1->SetMargin(0.25);
    lg1->SetFillColor(kWhite); lg1->Draw();
padhigh->SetTicks(1,1);

c1->cd();

	TPad* padlow = new TPad("padlow","padlow",0,0,1,0.2);
	padlow->Draw();
	padlow->SetBottomMargin(0.4);
    padlow->SetRightMargin(0.05);
    padlow->SetLeftMargin(0.12);
    padlow->SetTopMargin(0.01);
    padlow->SetGrid(0,1);
	
	TH1F * hratiomc = (TH1F*)hratio->Clone("hratiomc");
	TH1F * hratiomc2 = (TH1F*)hratio->Clone("hratiomc2");
	hratiomc->Reset(); hratiomc2->Reset();

	float r, err;
	for(int ii=1;ii<nbins+1;ii++){
	  if(hmc->GetBinContent(ii)) {r=hd->GetBinContent(ii)/hmc->GetBinContent(ii); err=hd->GetBinError(ii)/hmc->GetBinContent(ii);}
	  else {r=1; err=1;}
	  if(hd->GetBinContent(ii)==0) {r=-1; err=0;}
	  hratio->SetBinContent(ii,r); hratio->SetBinError(ii,err);
	  hratiomc->SetBinContent(ii,1); hratiomc2->SetBinContent(ii,1);
	  if(hmc->GetBinContent(ii)>0) {
	    hratiomc->SetBinError(ii,hmc->GetBinError(ii)/hmc->GetBinContent(ii));
		hratiomc2->SetBinError(ii,2.*hmc->GetBinError(ii)/hmc->GetBinContent(ii));
	  }
	  else {hratiomc->SetBinError(ii,1); hratiomc2->SetBinError(ii,2);}
	}
	hratiomc->GetYaxis()->SetNdivisions(503);
	hratiomc->GetYaxis()->SetLabelSize(0.18);
    hratiomc->GetYaxis()->SetTitleSize(0.18);
    hratiomc->GetYaxis()->SetTitleOffset(0.2);
	hratiomc->GetYaxis()->CenterTitle();
	
    hratiomc->GetXaxis()->SetLabelSize(0.18);
    hratiomc->GetXaxis()->SetTitleSize(0.18);
    //hratio->GetXaxis()->SetLabelOffset(0.08);
	
	hratiomc->SetFillStyle(3254);
	hratiomc->SetFillColor(TColor::GetColor("#99d8c9"));
	hratiomc->SetLineColor(1);
	hratiomc->SetLineWidth(2);

	padlow->cd();
	hratio->SetLineWidth(2);


	hratio->SetMarkerStyle(20);
	hratio->SetMarkerSize(1.2);
	hratio->SetMarkerColor(kBlack);
	hratio->SetLineColor(kBlack);
	fr->SetLineColor(kBlack);
		hratiomc->SetMaximum(maxRatio);
		hratiomc->SetMinimum(minRatio);	
	hratiomc->SetTitle(";"+var_name+";SF");
	if(xlogplot) hratiomc->GetXaxis()->SetMoreLogLabels(1);
	hratiomc->SetMarkerSize(0); hratiomc2->SetMarkerSize(0);
	//hratiomc2->Draw("E2,same");
	hratiomc->Draw("E2");
		TLegend* lg2 = new TLegend(0.125,0.769231,0.278894,0.944056,"");
		lg2->AddEntry(hratiomc,"Stat. uncertainty","F");
        lg2->SetBorderSize(0);
		lg2->SetMargin(0.25);
		lg2->SetFillStyle(0);
		lg2->SetTextSize(0.12);
		lg2->Draw("sames");	
	    hratio->Draw("same");
		TLine* rl = new TLine();
		rl->SetLineColor(kBlack);
		double nbinsX_r=hratio->GetNbinsX();
		rl->DrawLine(hratio->GetBinLowEdge(1), 1., hratio->GetBinLowEdge(nbinsX_r+1), 1.);
		if(xlogplot) c1->cd(2)->SetLogx();
		c1->cd(2)->SetTicky(); c1->cd(2)->SetTickx();
		padlow->RedrawAxis();		
		

c1->SaveAs("eff_"+filename+".png");	
c1->SaveAs("Plots/pdf/sf_"+filename+".pdf");	


}