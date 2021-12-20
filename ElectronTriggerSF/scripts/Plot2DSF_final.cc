void Plot2DSF_final(TString era=""){
	
gStyle->SetErrorX(.5); // somthing strange with Draw("E2")
char str[100];
double GeV=1e3, norm;
gStyle->SetOptStat(0);
gStyle->SetPaintTextFormat("2.2f");

TExec *setex1 = new TExec("setex1","gStyle->SetErrorX(0)");
TExec *setex2 = new TExec("setex2","gStyle->SetErrorX(.5)");


TPaveText * clumi = new TPaveText(0.295829,0.776853,0.388191,0.9131668,"NDC");
clumi->SetFillColor(0); clumi->SetLineColor(0);
clumi->SetTextFont(42); clumi->SetTextSize(0.0499401); clumi->SetBorderSize(0);
double maxRatio=1.2, minRatio=0.8;

//TString path="./";
TString path="/eos/user/m/mpitt/PPS_ttbar/ElTrigger";
//TString era = "C"; // can be empty 
TString var, var_name, filename = "final"+era; 
TCut cut = "HLT_IsoMu27 && nano_nElectrons==1 &&  nano_nBJets>1 && nano_ElPT[0]>30 && (nano_ElID[0] & (1 << 5)>0) && abs(nano_ElEta[0])<2.1 && nano_ElMatch[0]";
TCut cut2 = cut && "nano_nJets>3";
TString trigger_list, TrigNUM, TrigDEN;
TString label1, label2, label3, label4;

TString infilemc = "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root";
TString infiledatafull = "Run2017.root";
TString infiledata = Form("Run2017%s.root",era.Data());

const int nPTBins=8; double PTBounds[nPTBins+1] ={30,35,40,50,60,80,100,150,500};
const int nEtaBins=8; double EtaBounds[nEtaBins+1] ={-2.1,-1.566,-1.4442,-0.8,0,0.8,1.4442,1.566,2.1};

trigger_list = "HLT_Ele35_WPTight_Gsf || HLT_Ele28_eta2p1_WPTight_Gsf_HT150 || HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned";
TrigNUM="Ele35_or_Ele28HT150_or_Ele30CentralPFJet35";
TrigDEN="CutBasedId";
//trigger_list = "HLT_Ele28_eta2p1_WPTight_Gsf_HT150";

// output
TString nomname="NUM_"+TrigNUM+"_DEN_"+TrigDEN+"_eta_pt";
TString file_refix="data/Efficiencies_electron_DL_Run2017";
TString file_suffix="_UL_SingleElectronCrossTriggers.root";


// Get Systematic error:
cout << "calculate systematic errors"<<endl;
TChain * t_datafull = new TChain("Events"); t_datafull->Add(path+"/"+infiledatafull);
TProfile2D * hdfull = new TProfile2D("hdfull","",nPTBins,PTBounds,nEtaBins,EtaBounds);
TProfile2D * hdfull2 = new TProfile2D("hdfull2","",nPTBins,PTBounds,nEtaBins,EtaBounds);

t_datafull->Draw(Form("%s :  nano_ElEta[0]: nano_ElPT[0] >>hdfull",trigger_list.Data()), cut );
t_datafull->Draw(Form("%s :  nano_ElEta[0]: nano_ElPT[0] >>hdfull2",trigger_list.Data()), cut2 );

// define trees and histograms
cout << "Obtain SF for "<<(era.IsWhitespace()? "full 2017" : "period = "+era) << endl;
TChain * t_mc = new TChain("Events"); t_mc->Add(path+"/"+infilemc);
TChain * t_data = new TChain("Events"); t_data->Add(path+"/"+infiledata);

TProfile2D * hmc = new TProfile2D("hmc","",nPTBins,PTBounds,nEtaBins,EtaBounds);
TProfile2D * hd = new TProfile2D("hd","",nPTBins,PTBounds,nEtaBins,EtaBounds);
TH2F * hratio = new TH2F("hratio",";Electron p_{T} [GeV];Electron #eta",nPTBins,PTBounds,nEtaBins,EtaBounds);
TH2F * hratio_stat = new TH2F("hratio_stat",";Electron p_{T} [GeV];Electron #eta",nPTBins,PTBounds,nEtaBins,EtaBounds);
TH2F * hratio_syst = new TH2F("hratio_syst",";Electron p_{T} [GeV];Electron #eta",nPTBins,PTBounds,nEtaBins,EtaBounds);


// plotting efficiency curves:

t_mc->Draw(Form("%s : nano_ElEta[0]: nano_ElPT[0] >>hmc",trigger_list.Data()), cut );
t_data->Draw(Form("%s :  nano_ElEta[0]: nano_ElPT[0] >>hd",trigger_list.Data()), cut );


TCanvas* c1 = new TCanvas("c1","c1",0,50,1000,600);

c1->SetTicks(1,1);
c1->SetBottomMargin(0.12);
c1->SetLeftMargin(0.08);
c1->SetTopMargin(0.060);
c1->SetRightMargin(0.10);
c1->SetTicks(0,1);

// Load run dependent SF for entire year systematics
TH2F * sf_rundependent[5];
float LumiWeights[5]={0.116, 0.232, 0.102, 0.223, 0.326};
TString eras[5]={"B","C","D","E","F"};
if(era.IsWhitespace()){
cout << "Add run-dependent systematics" << endl;
for (int i=0;i<5;i++){
	TFile * frun = TFile::Open(file_refix+eras[i]+file_suffix);
	sf_rundependent[i] = ((TH2F*)frun->Get(nomname)->Clone("sf_rundependent"+eras[i]));
}}

// Obtain SF + systematic errors:
float r, err;	
for (int ix=0;ix<nPTBins;ix++){
	float x = 0.5*(PTBounds[ix]+PTBounds[ix+1]);
	for (int iy=0;iy<nEtaBins;iy++){
		float y = 0.5*(EtaBounds[iy]+EtaBounds[iy+1]);
		int bin = hmc->FindBin(x,y);
	  if(hmc->GetBinContent(bin)) {r=hd->GetBinContent(bin)/hmc->GetBinContent(bin); err=hd->GetBinError(bin)/hmc->GetBinContent(bin);}
      else {
		  hratio->SetBinContent(bin,-1); hratio->SetBinError(bin,1); 
		  hratio_stat->SetBinContent(bin,-1); hratio_stat->SetBinError(bin,1); 
		  hratio_syst->SetBinContent(bin,-1); hratio_syst->SetBinError(bin,1); 
		  continue;
	  }
	  if(hd->GetBinContent(bin)==0) {
		  hratio->SetBinContent(bin,-1); hratio->SetBinError(bin,0); 
		  hratio_stat->SetBinContent(bin,-1); hratio_stat->SetBinError(bin,0); 
		  hratio_syst->SetBinContent(bin,-1); hratio_syst->SetBinError(bin,0); 
		  continue;
	  }
	  // systematic errors:
	  // error from difference between nominal and high nJet region
	  float sys_err1 = hdfull->GetBinContent(bin) - hdfull2->GetBinContent(bin);
	  sys_err1 = abs(sys_err1) / hmc->GetBinContent(bin);
	  
	  // Systematic uncertanty for lumi-weighted run dependent SFs (only derived for full 2017)
	  float sys_err2 = 0, lumi_w_sf=0;
	  if(era.IsWhitespace()){
		  for (int i=0;i<5;i++){
			  int bin2 = sf_rundependent[i]->FindBin(y,x);
			  float sfera=sf_rundependent[i]->GetBinContent(bin2);
			  lumi_w_sf+=LumiWeights[i]*sfera;
		  }
		  sys_err2= r - lumi_w_sf;
	  }		  

	  hratio_stat->SetBinContent(bin,r); hratio_stat->SetBinError(bin,err);
	  hratio_syst->SetBinContent(bin,r); hratio_syst->SetBinError(bin,sqrt(sys_err1*sys_err1+sys_err2*sys_err2));
	  hratio->SetBinContent(bin,r); hratio->SetBinError(bin,sqrt(err*err + sys_err1*sys_err1+sys_err2*sys_err2));
	}
}
	

//clumi->AddText("TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8");

hratio->SetMaximum(1);
hratio->SetMinimum(0);
hratio->GetYaxis()->SetTitleOffset(0.8);   
hratio->GetYaxis()->SetTitleSize(0.045);
hratio->GetYaxis()->SetLabelSize(0.035);
hratio->GetXaxis()->SetTitleOffset(1.4);   
hratio->Draw("colztextE");


//clumi->Draw();

	TLatex * text_cms = new TLatex();
	text_cms->SetNDC(true);
    text_cms->SetTextFont(42);
    text_cms->SetTextSize(0.045);
    text_cms->SetTextAlign(12)	;
	text_cms->DrawLatex(0.08,0.97,"#bf{CMS} #it{Internal}"); 
    text_cms->SetTextSize(0.040);
	text_cms->SetTextAlign(kHAlignRight+kVAlignCenter);
	text_cms->DrawLatex(0.90,0.97,Form("#scale[0.9]{%s (13 TeV)}",("2017"+era).Data()));
	

c1->cd();
hratio->GetXaxis()->SetMoreLogLabels(1);
c1->SetLogx();

c1->SaveAs("Plots/pdf/sf2D_"+filename+".pdf");	

// Save histograms in output file (pt,eta) -> (eta,pt):
if(era=="") era="BCDEF";
TFile * fileout = new TFile(file_refix+era+file_suffix,"recreate");

TH2F * hsf = new TH2F(nomname,";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * hsf_stat = new TH2F(nomname+"_stat",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * hsf_syst = new TH2F(nomname+"_syst",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * heffData = new TH2F(nomname+"_efficiencyData",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * heffData_stat = new TH2F(nomname+"_efficiencyData_stat",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * heffData_syst = new TH2F(nomname+"_efficiencyData_syst",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * heffMC = new TH2F(nomname+"_efficiencyMC",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * heffMC_stat = new TH2F(nomname+"_efficiencyMC_stat",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);
TH2F * heffMC_syst = new TH2F(nomname+"_efficiencyMC_syst",";Electron #eta;Electron p_{T} [GeV]",nEtaBins,EtaBounds,nPTBins,PTBounds);

for (int ix=0;ix<nPTBins;ix++){
	float pt = 0.5*(PTBounds[ix]+PTBounds[ix+1]);
	for (int iy=0;iy<nEtaBins;iy++){
		float eta = 0.5*(EtaBounds[iy]+EtaBounds[iy+1]);
		
		int binin = hmc->FindBin(pt,eta);
		int binout = hsf->FindBin(eta,pt);
		
		// sf
		if(hratio->GetBinContent(binin)>0){
			hsf->SetBinContent(binout,hratio->GetBinContent(binin));
			hsf_stat->SetBinContent(binout,hratio_stat->GetBinContent(binin));
			hsf_syst->SetBinContent(binout,hratio_syst->GetBinContent(binin));
		}
		else{ // (change back from -1 to 0)
			hsf->SetBinContent(binout,0);
			hsf_stat->SetBinContent(binout,0);
			hsf_syst->SetBinContent(binout,0);
		}

		hsf->SetBinError(binout,hratio->GetBinError(binin));
		hsf_stat->SetBinError(binout,hratio_stat->GetBinError(binin));
		hsf_syst->SetBinError(binout,hratio_syst->GetBinError(binin));
		
		// data
		heffData->SetBinContent(binout,hd->GetBinContent(binin));
		heffData_stat->SetBinContent(binout,hd->GetBinContent(binin));
		heffData_syst->SetBinContent(binout,hd->GetBinContent(binin));
		
		heffData_stat->SetBinError(binout,hd->GetBinError(binin));
		err = hdfull->GetBinContent(binin) - hdfull2->GetBinContent(binin);
		heffData_syst->SetBinError(binout,abs(err));
		heffData->SetBinError(binout,sqrt(err*err+hd->GetBinError(binin)*hd->GetBinError(binin)));
		
		// MC
		heffMC->SetBinContent(binout,hd->GetBinContent(binin));
		heffMC_stat->SetBinContent(binout,hd->GetBinContent(binin));
		heffMC_syst->SetBinContent(binout,hd->GetBinContent(binin));
		
		heffMC_stat->SetBinError(binout,hmc->GetBinError(binin));
		heffMC_syst->SetBinError(binout,hmc->GetBinError(binin));
		heffMC->SetBinError(binout,hmc->GetBinError(binin));	
	}
}
		
hsf->Write();
hsf_stat->Write();
hsf_syst->Write();	
heffData->Write();
heffData_stat->Write();
heffData_syst->Write();
heffMC->Write();
heffMC_stat->Write();
heffMC_syst->Write();

fileout->Write();
cout << "Writes " << fileout->GetName() << endl;
fileout->Close();

}