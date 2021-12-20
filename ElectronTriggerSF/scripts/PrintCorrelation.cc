{
	
TString path="/eos/user/m/mpitt/PPS_ttbar/ElTrigger";

TCut cut = "nano_nElectrons==1 &&  nano_nBJets>1   ";
cut = cut && "nano_ElPT[0]>30 && (nano_ElID[0] & (1 << 5)>0) && abs(nano_ElEta[0])<2.1";
TString infilemc = "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root";


TCut trigMuon = "HLT_IsoMu27";
const int nTrigger = 4;
TCut trigEle[nTrigger] = {
	"HLT_Ele35_WPTight_Gsf",
	"HLT_Ele28_eta2p1_WPTight_Gsf_HT150",
	"HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned",
	"(HLT_Ele35_WPTight_Gsf || HLT_Ele28_eta2p1_WPTight_Gsf_HT150 || HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned)"
};

TChain * t_mc = new TChain("Events"); t_mc->Add(path+"/"+infilemc);

float Ntotal = t_mc->GetEntries(cut);

cout << "Total considered events = " << int(Ntotal) << endl;

float N_mu = t_mc->GetEntries(cut && trigMuon);
cout << "eff mu = " << N_mu/Ntotal << endl;

cout << endl << "Calculate trigger efficinecies: "<<endl;
float N_el, N_muel, err_;
for(int i=0;i<nTrigger;i++){
	cout << "For "<<trigEle[i]<<endl;
	N_el = t_mc->GetEntries(cut && trigEle[i]);
	cout <<"eff el = " << N_el/Ntotal<< endl;
	N_muel = t_mc->GetEntries(cut && trigEle[i] && trigMuon);
	cout <<"eff elANDmu = " << N_muel/Ntotal<< endl;
	err_ = TMath::Sqrt( N_mu + N_el + N_muel)/Ntotal;
    printf("Correlation factor = %2.5f +/- %2.5f\n",(N_mu/N_muel)*(N_el/Ntotal),err_);
}



	
	
	
	
	
}