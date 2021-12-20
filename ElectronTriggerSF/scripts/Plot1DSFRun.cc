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
//TCut cut = "nano_nElectrons==1 &&  nano_nBJets>1&&  nano_nJets>3";
TCut cut = "nano_nElectrons==1 &&  nano_nBJets>1";
cut = cut && "nano_ElPT[0]>30 && (nano_ElID[0] & (1 << 5)>0) && abs(nano_ElEta[0])<2.1 && nano_ElMatch[0]";

TString trigger_list;
TString label1, label2, label3, label4;
float xmin=0, xmax=1;
TString infilemc = "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root";
TString infiledata = "Run2017.root";

bool fitme = false; bool specialBins=false, logx = false;

//var = "nano_ElPT[0]"; var_name = "Electron p_{T} [GeV]"; filename="pt";logx = true;
//const Int_t nbins = 8; Double_t Bins[nbins+1]={30,35,40,50,60,80,100,150,500}; specialBins=true;


var = "nano_ElEta[0]"; var_name = "Electron #eta"; filename="eta";
const Int_t nbins = 8; Double_t Bins[nbins+1]={-2.1,-1.566,-1.4442,-0.8,0,0.8,1.4442,1.566,2.1}; specialBins=true;


//var = "nano_ElEta[0]"; var_name = "Electron #eta"; const int nbins=44; xmin=-2.1; xmax=2.1; filename="eta";
//var = "(nano_HT+nano_ElPT[0])/1e3"; var_name = "#Sigmap_{T}^{jet}+p_{T}^{el} [TeV]"; const int nbins=25; xmin=0.075; xmax=1.075; filename="ht";
//var = "nano_nJets"; var_name = "jet multiplicity"; const int nbins=6; xmin=3.5; xmax=9.5; filename="jetn";


//trigger_list = "HLT_Ele35_WPTight_Gsf || HLT_Ele28_eta2p1_WPTight_Gsf_HT150 || HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned";
trigger_list = "HLT_Ele32_WPTight_Gsf_L1DoubleEG";

if(!specialBins){
	for (int i=0;i<nbins+1;i++) {Bins[i]=xmin+i*(xmax-xmin)/(nbins);}
}

TChain * t_mc = new TChain("Events"); t_mc->Add(path+"/"+infilemc);
TChain * t_data = new TChain("Events"); t_data->Add(path+"/"+infiledata);

float r, err;

// define trees and histograms
TProfile * hmc = new TProfile("hmc",";"+var_name+";Efficiency",nbins,Bins,-0.5,1.5);
TProfile * hd = new TProfile("hd","",nbins,Bins,-0.5,1.5);
TH1F * hsf = new TH1F("hsf","",nbins,Bins);

t_mc->Draw(Form("%s : %s>>hmc",trigger_list.Data(),var.Data()), cut );
t_data->Draw(Form("%s : %s>>hd",trigger_list.Data(),var.Data()), cut );
for(int ii=1;ii<nbins+1;ii++){
   if(hmc->GetBinContent(ii)) {r=hd->GetBinContent(ii)/hmc->GetBinContent(ii); err=hd->GetBinError(ii)/hmc->GetBinContent(ii);}
	else {r=1; err=1;}
	if(hd->GetBinContent(ii)==0) {r=-1; err=0;}
	hsf->SetBinContent(ii,r); hsf->SetBinError(ii,err);
}

// split to different runs
TChain * t_dataEra[5]; TString eras[5]={"B","C","D","E","F"};
TProfile * hdEra[5]; TH1F * hsfEra[5];
for (int i=0;i<5;i++){
	t_dataEra[i] = new TChain("Events");
	t_dataEra[i]->Add(path+"/Run2017"+eras[i]+".root");	
	hdEra[i] = new TProfile(Form("hd%d",i),"",nbins,Bins,-0.5,1.5);
	hsfEra[i] = new TH1F(Form("hsf%d",i),"",nbins,Bins);
}
for (int i=0;i<5;i++){
	t_dataEra[i]->Draw(Form("%s : %s>>hd%d",trigger_list.Data(),var.Data(),i), cut );
	for(int ii=1;ii<nbins+1;ii++){
		if(hmc->GetBinContent(ii)) {r=hdEra[i]->GetBinContent(ii)/hmc->GetBinContent(ii); err=hdEra[i]->GetBinError(ii)/hmc->GetBinContent(ii);}
	    else {r=1; err=1;}
	    if(hdEra[i]->GetBinContent(ii)==0) {r=-1; err=0;}
	    hsfEra[i]->SetBinContent(ii,r); hsfEra[i]->SetBinError(ii,err);
    }
}


TCanvas* c1 = new TCanvas("c1","c1",0,50,600,600);

c1->SetTicks(1,1);
c1->SetGrid(1,1);
c1->SetLeftMargin(0.1);
c1->SetRightMargin(0.03);
c1->SetTopMargin(0.073);

hsf->SetMarkerStyle(20);
hsf->SetMarkerColor(kBlack);
hsf->SetLineColor(kBlack);
hsf->SetLineWidth(2);

for (int i=0;i<5;i++){
hsfEra[i]->SetMarkerStyle(21+i);
hsfEra[i]->SetMarkerColor(i+2);
hsfEra[i]->SetLineColor(i+2);
hsfEra[i]->SetLineWidth(2);
}

//clumi->AddText("TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8");
if(logx) hsf->GetXaxis()->SetMoreLogLabels();

hsf->GetYaxis()->SetRangeUser(0.5,1.5);
hsf->Draw();
for (int i=0;i<5;i++){ hsfEra[i]->Draw("same");}


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
lg1->AddEntry(hsf,"2017BCDEF","LP");
for (int i=0;i<5;i++){
	lg1->AddEntry(hsfEra[i],Form("2017%s",eras[i].Data()),"LP");
}
    lg1->SetTextFont(42); lg1->SetTextSize(0.033);
    lg1->SetBorderSize(0);
    lg1->SetMargin(0.25);
    lg1->SetFillColor(kWhite); lg1->Draw();
c1->SetTicks(1,1);
if(logx) c1->SetLogx();

c1->SaveAs("Plots/pdf/sf_runsys_"+filename+".pdf");	


}