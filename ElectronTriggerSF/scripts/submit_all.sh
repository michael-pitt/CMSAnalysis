python scripts/processDataset.py --submit #default is muon
python scripts/processDataset.py -i /HighEGJet/Run2017H-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD -o /eos/user/p/psilva/data/sdanalysis/HighEGJet/Chunks -t HLT_HIEle15_WPLoose_Gsf -a analysis_el --submit
python scripts/processDataset.py -i /HighEGJet/Run2017H-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD -o /eos/user/p/psilva/data/sdanalysis/HighEGJet_emu/Chunks -t HLT_HIEle15_WPLoose_Gsf -a analysis_emu --submit
