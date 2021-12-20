{

TCut trigMuon = "HLT_IsoMu27";
TCut trigEle = "(HLT_Ele35_WPTight_Gsf || HLT_Ele28_eta2p1_WPTight_Gsf_HT150 || HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned)";
	
TString path="/eos/user/m/mpitt/PPS_ttbar/ElTrigger";

TCut cut = "nano_nElectrons==1 &&  nano_nBJets>1   ";
cut = cut && "nano_ElPT[0]>30 && (nano_ElID[0] & (1 << 5)>0) && abs(nano_ElEta[0])<2.1";
TString infilemc = "SingleMuon_Run2017";
const int nEras = 5;
TString eras[nEras] = {"B","C","D","E","F"};

TChain * t_mc = new TChain("Events"); t_mc->Add(path+"/"+infilemc+".root");
float Ntotal = t_mc->GetEntries(cut);

float N_mu = t_mc->GetEntries(cut && trigMuon);
float N_el = t_mc->GetEntries(cut && trigEle);
float N_muel = t_mc->GetEntries(cut && trigEle && trigMuon);

cout << "\t\tfraction of reference\t\tfraction of signal\t\t\tglobal efficiency"<<endl;
for(int i=0;i<nEras;i++){
	TChain * t_mcera = new TChain("Events"); t_mcera->Add(path+"/"+infilemc+eras[i]+".root");

	float Ntotaleta = t_mcera->GetEntries(cut);

	float N_muera = t_mcera->GetEntries(cut && trigMuon);
    float N_elera = t_mcera->GetEntries(cut && trigEle);
    float N_muelera = t_mcera->GetEntries(cut && trigEle && trigMuon);

    printf("%s %2.3f & %2.3f & %2.3f \\\\ \n",eras[i].Data(),(N_muera/N_mu),(N_elera/N_el),(N_muelera/N_muera));
}

    printf("BCDEF 1.000 & 1.000 & %2.3f \\\\ \n",(N_muel/N_mu));

cout << "Done" << endl;
	
	
	
	
	
}