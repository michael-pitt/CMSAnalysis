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
TCut cut = "nano_nElectrons==1 &&  nano_nBJets>1";
cut = cut && "(nano_ElID[0] & (1 << 5)>0) && abs(nano_ElEta[0])<2.1 && nano_ElMatch[0]";
TCut cut2 = "nano_nElectrons==1 &&  nano_nBJets>1&&  nano_nJets>3 ";
cut2 = cut2 && "(nano_ElID[0] & (1 << 5)>0) && abs(nano_ElEta[0])<2.1 && nano_ElMatch[0]";
TString text_legend = "N_{jet}#geq4", prefix = "jetN";
TString trigger_list;
TString label1, label2, label3, label4;
float xmin=0, xmax=1;
TString infilemc = "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root";
TString infiledata = "Run2017.root";
bool fitme = false, specialBins=false, xlogplot= false;

//var = "nano_ElPT[0]"; var_name = "Electron p_{T} [GeV]"; filename="pt";xlogplot = true;
//const Int_t nbins = 8; Double_t Bins[nbins+1]={30,35,40,50,60,80,100,150,500}; specialBins=true;


var = "nano_ElEta[0]"; var_name = "Electron #eta"; filename="eta";
const Int_t nbins = 8; Double_t Bins[nbins+1]={-2.1,-1.566,-1.4442,-0.8,0,0.8,1.4442,1.566,2.1}; specialBins=true;

//var = "(nano_HT+nano_ElPT[0])/1e3"; var_name = "#Sigmap_{T}^{jet}+p_{T}^{el} [TeV]"; nbins=25; xmin=0.075; xmax=1.075; filename="ht";
//var = "nano_nJets"; var_name = "jet multiplicity"; nbins=6; xmin=3.5; xmax=9.5; filename="jetn";


trigger_list = "HLT_Ele35_WPTight_Gsf || HLT_Ele28_eta2p1_WPTight_Gsf_HT150 || HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned";
//trigger_list = "HLT_Ele28_eta2p1_WPTight_Gsf_HT150";

if(!specialBins){
	for (int i=0;i<nbins+1;i++) {Bins[i]=xmin+i*(xmax-xmin)/(nbins);}
}

TChain * t_mc = new TChain("Events"); t_mc->Add(path+"/"+infilemc);
TChain * t_data = new TChain("Events"); t_data->Add(path+"/"+infiledata);

filename=prefix+"_"+filename;
float r, err;

// define trees and histograms
TProfile * hmc1 = new TProfile("hmc1",";"+var_name+";Efficiency",nbins,Bins,-0.5,1.5);
TProfile * hd1 = new TProfile("hd1","",nbins,Bins,-0.5,1.5);
TH1F * hratio = new TH1F("hratio","",nbins,Bins);
TH1F * hsf1 = new TH1F("hsf1","",nbins,Bins);

t_mc->Draw(Form("%s : %s>>hmc1",trigger_list.Data(),var.Data()), cut );
t_data->Draw(Form("%s : %s>>hd1",trigger_list.Data(),var.Data()), cut );
for(int ii=1;ii<nbins+1;ii++){
   if(hmc1->GetBinContent(ii)) {r=hd1->GetBinContent(ii)/hmc1->GetBinContent(ii); err=hd1->GetBinError(ii)/hmc1->GetBinContent(ii);}
	else {r=1; err=1;}
	if(hd1->GetBinContent(ii)==0) {r=-1; err=0;}
	hsf1->SetBinContent(ii,r); hsf1->SetBinError(ii,err);
}
	
// define trees and histograms
TProfile * hmc2 = new TProfile("hmc2",";"+var_name+";Trigger SF",nbins,Bins,-0.5,1.5);
TProfile * hd2 = new TProfile("hd2","",nbins,Bins,-0.5,1.5);
TH1F * hsf2 = new TH1F("hsf2","",nbins,Bins);

t_mc->Draw(Form("%s : %s>>hmc2",trigger_list.Data(),var.Data()), cut2 );
t_data->Draw(Form("%s : %s>>hd2",trigger_list.Data(),var.Data()), cut2 );
for(int ii=1;ii<nbins+1;ii++){
   if(hmc2->GetBinContent(ii)) {r=hd2->GetBinContent(ii)/hmc2->GetBinContent(ii); err=hd2->GetBinError(ii)/hmc2->GetBinContent(ii);}
	else {r=1; err=1;}
	if(hd2->GetBinContent(ii)==0) {r=-1; err=0;}
	hsf2->SetBinContent(ii,r); hsf2->SetBinError(ii,err);
}

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

hsf1->SetMarkerStyle(20);
hsf1->SetMarkerColor(kBlack);
hsf1->SetLineColor(kBlack);
hsf1->SetLineWidth(2);

hsf2->SetMarkerStyle(21);
hsf2->SetMarkerColor(kGreen+2);
hsf2->SetLineColor(kGreen+2);
hsf2->SetLineWidth(2);

//clumi->AddText("TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8");

hsf1->GetYaxis()->SetRangeUser(0.5,1.5);
hsf1->Draw();
hsf2->Draw("same");


//clumi->Draw();

	TLatex * text_cms = new TLatex();
	text_cms->SetNDC(true);
    text_cms->SetTextFont(42);
    text_cms->SetTextSize(0.045);
    text_cms->SetTextAlign(12)	;
	text_cms->DrawLatex(0.14,0.97,"#bf{CMS} #it{Internal}"); 
    text_cms->SetTextSize(0.040);
	text_cms->SetTextAlign(kHAlignRight+kVAlignCenter);
	text_cms->DrawLatex(0.95,0.97,Form("#scale[0.9]{%3.1f fb^{-1} (13 TeV)}",41.5));
	
TLegend* lg1 = new TLegend(0.133779,0.709059,0.40,0.88676,"");
lg1->AddEntry(hsf1,"nominal selection","LP");
lg1->AddEntry(hsf2,text_legend,"LP");
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

	for(int ii=1;ii<nbins+1;ii++){
	  if(hsf1->GetBinContent(ii)) {r=hsf2->GetBinContent(ii)/hsf1->GetBinContent(ii); err=hsf2->GetBinError(ii)/hsf1->GetBinContent(ii);}
	  else {r=1; err=1;}
	  if(hsf2->GetBinContent(ii)==0) {r=-1; err=0;}
	  hratio->SetBinContent(ii,r); hratio->SetBinError(ii,err);
	  hratiomc->SetBinContent(ii,1); hratiomc2->SetBinContent(ii,1);
	  if(hsf1->GetBinContent(ii)>0) {
	    hratiomc->SetBinError(ii,hsf1->GetBinError(ii)/hsf1->GetBinContent(ii));
		hratiomc2->SetBinError(ii,2.*hsf1->GetBinError(ii)/hsf1->GetBinContent(ii));
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
	hratio->SetMarkerColor(kGreen+2);
	hratio->SetLineColor(kGreen+2);
		hratiomc->SetMaximum(maxRatio);
		hratiomc->SetMinimum(minRatio);	
	hratiomc->SetTitle(";"+var_name+";ratio");
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
		if(xlogplot) {padhigh->SetLogx();padlow->SetLogx();}
		c1->cd(2)->SetTicky(); c1->cd(2)->SetTickx();
		padlow->RedrawAxis();		



c1->SaveAs("Plots/pdf/sf_sys_"+filename+".pdf");	


}