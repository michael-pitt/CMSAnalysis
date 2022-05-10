class ObjectSelector:
    def __init__(self, _year = "None" ):
        self.year = _year


class ElectronSelector(ObjectSelector):
    def __init__(self, minPt = 25):
        self.minPt = minPt

    def evalElectron(self, el):
        
        isEBEE = True if abs(el.eta)>1.4442 and abs(el.eta)<1.5660 else False
        
        if isEBEE: return False
        if el.pt < self.minPt: return False
        if abs(el.eta) > 2.4: return False
        if abs(el.eta)<1.479 and (abs(el.dxy) > 0.05 or abs(el.dz) > 0.1): return False
        if abs(el.eta)>1.479 and (abs(el.dxy) > 0.10 or abs(el.dz) > 0.2): return False
        #if not el.mvaFall17V2noIso_WP80: return False
        #if el.cutBased<3: return False

        return True
        
class MuonSelector(ObjectSelector):
    def __init__(self, minPt = 25, id = 'medium'):
        self.minPt = minPt
        self.id = id

    def evalMuon(self, mu):

        if mu.pt < self.minPt: return False
        if abs(mu.eta) > 2.4: return False
        if mu.pfRelIso04_all>0.4: return False
        #if abs(mu.dxybs) > 0.05 or abs(mu.dz) > 1.0: return False
        if abs(mu.dxybs) > 0.05: return False
        if self.id == 'tight' and not mu.tightId: return False
        elif self.id == 'medium' and not mu.mediumId: return False
        elif self.id == 'loose' and not mu.looseId: return False
        return True
        
