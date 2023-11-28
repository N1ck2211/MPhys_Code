#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <ROOT/RVec.hxx>
#include <fstream>

void add_branch(){

    TFile f("wzp6_ee_Hgg_ecm125.root", "update");

    Float_t new_class;
    auto events = f.Get<TTree>("events");
    auto newBranch = events->Branch("new_class", &new_class, "new_class/F");

    Long64_t nentries = events->GetEntries();

    for (Long64_t i = 0; i < nentries; i++) {
        new_class = 0.0;
        newBranch->Fill();
    }

    events->Write("", TObject::kOverwrite);
}

int main(){

    add_branch();

    return 0;

}
