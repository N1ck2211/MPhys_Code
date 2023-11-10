#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <ROOT/RVec.hxx>

using namespace std;

void readTree(const char* fileName, const char* treeName, const char* branchName) {
    // Open the ROOT file
    TFile *file = new TFile(fileName);

    ofstream outputFile;
    outputFile.open("output.csv");

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
    // Assuming the branch contains integers, adjust the type accordingly
    
    // If the data is a float type
    float_t branchValue;

    // If the data is a RVec float type
    // ROOT::VecOps::RVec<float> *branchValue;
    tree->SetBranchAddress(branchName, &branchValue);

    // Get the number of entries in the tree
    Long64_t nEntries = tree->GetEntries();

    // Loop over entries
    for (Long64_t iEntry = 0; iEntry < 5; ++iEntry) {
        // Read the entry
        tree->GetEntry(iEntry);

        // Access the branch value and write to a file
        std::cout << "Entry " << iEntry << ", " << branchName << " = " << branchValue << std::endl;

        outputFile << branchValue << std::endl;
    }
    
    //Only necessary when using RVec float type:
    //delete branchValue;

    // Close the file
    file->Close();
    outputFile.close();
}

int main() {
    const char* fileName = "wzp6_ee_Hgg_ecm125.root";
    const char* treeName = "events";
    const char* branchName = "jj_m";

    readTree(fileName, treeName, branchName);

    return 0;
}
