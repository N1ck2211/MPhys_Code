#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <ROOT/RVec.hxx>
#include <fstream>
using namespace std;

void readTree(const char* fileName, const char* treeName, const char* branchName) {
    ofstream outputfile;

    // Open the ROOT file
    TFile *file = new TFile(fileName);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open file " << fileName << std::endl;
        return;
    }

    // Access the TTree
    TTree *tree = (TTree*)file->Get(treeName);
    if (!tree) {
        std::cerr << "Error: Cannot find tree " << treeName << " in file " << fileName << std::endl;
        file->Close();
        return;
    }

    // Set the branch address
    // Assuming the branch contains flaots:
   // float_t branchValue;
   // For RVEct values:

    ROOT::VecOps::RVec<float> *branchValue = nullptr;
    tree->SetBranchAddress(branchName, &branchValue);

    // Get the number of entries in the tree

    Long64_t nEntries = tree->GetEntries();
    outputfile.open("output.csv");
    
    float_t out_val;
    // Loop over entries
    for (Long64_t iEntry = 0; iEntry < nEntries * 0.1; ++iEntry) {
        // Read the entry
        tree->GetEntry(iEntry);

        out_val = branchValue[0];

        // Access the branch value
        std::cout << "Entry " << iEntry << ", " << branchName << " = " << out_val << std::endl;
        outputfile << out_val << "\n";

    }
    outputfile.close();

    // Close the file
    file->Close();
    delete branchValue;
}

int main() {
    const char* fileName = "wzp6_ee_Hgg_ecm125.root";
    const char* treeName = "events";
    const char* branchName = "jj_q";

    readTree(fileName, treeName, branchName);

    return 0;
}
