#include "TApplication.h"
#include <TSystemDirectory.h>
#include <TList.h>
#include <TCollection.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TTree.h>
#include <TString.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>

#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/DataLoader.h"


void TrainMVA(TChain *fChain);
void TrainMVAEvent(TChain *fChain);
void TrainMVAEventBoosted(TChain *fChain);
void TrainMultiClassMVA(TChain *fChain);
void Evaluate_MultiClassMVA(TChain *fChain);
std::vector<TTree*> GetTrees(TChain *fChain);
template <typename DataType> void RunCutOptimisation(TChain *fChain, TString observable);

int main(int argc, char** argv){
  clock_t begin = clock();
  TApplication theApp("evsel", &argc, argv);
  TString TreeName   =  theApp.Argv(1);
  TString SampleName =  theApp.Argv(2);
  ///TString observable =  theApp.Argv(3);  
  ///TString type       =  theApp.Argv(4);

  TChain* mych_data = new TChain (TreeName);
  mych_data->Add("inputs/MVATraining/"+SampleName);
  ///TrainMVA(mych_data);
  ///TrainMVAEvent(mych_data);
  TrainMVAEventBoosted(mych_data);
  ///TrainMultiClassMVA(mych_data);
  ///Evaluate_MultiClassMVA(mych_data);
  /*if(type=="int"){
     RunCutOptimisation<int>(mych_data,observable);
  }else if(type=="double"){
     RunCutOptimisation<double>(mych_data,observable);
  }*/
  return 0;
}

template <typename DataType> void RunCutOptimisation(TChain *fChain, TString observable){
    DataType m_observable;
    int      is_Signal;
    int      step_size = 20;
    double   x_range   = 20.;
    fChain->SetBranchAddress(observable, &m_observable);
    fChain->SetBranchAddress("is_Signal", &is_Signal);
    std::vector<int> selected_events_sig(step_size, 0);
    std::vector<int> selected_events_bkg(step_size, 0);
    for (Long64_t jentry=0; jentry<fChain->GetEntriesFast();jentry++) {
        fChain->GetEntry(jentry);   
        for(unsigned int i=0; i<step_size; i++){
            DataType m_cut = (DataType) ((i)*(x_range/step_size));
            if(is_Signal==1){
                if(m_observable >= m_cut && m_observable < m_cut + 1)selected_events_sig[i]++;
            }else if(is_Signal==0){
                if(m_observable >= m_cut && m_observable < m_cut + 1)selected_events_bkg[i]++;
            }
        }
    }
    /// find maximum 
    for(unsigned int i=0; i<step_size; i++){
        DataType m_cut      = (DataType) ((i)*(x_range/step_size));
        double significance = selected_events_sig[i]/sqrt(selected_events_bkg[i]);
        std::cout<< significance << "for >=" << m_cut <<std::endl;
    }
}

void TrainMVA(TChain *fChain){
   TFile *outputFile            = TFile::Open("output/TMVAResults_v2.root", "RECREATE" );
   TMVA::Factory *factory	= new TMVA::Factory("TMVAClassificationCategory", outputFile, "!V:!Silent:Transformations=I;P;G");
   TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

   /*
   dataloader->AddVariable("btagjH1", "btagjH1","",'D');
   dataloader->AddVariable("btagjH2", "btagjH2","",'D');
   dataloader->AddVariable("btagjW1", "btagjW1","",'D');
   dataloader->AddVariable("btagjW2", "btagjW2","",'D');
   dataloader->AddVariable("H_mass", "H_mass","",'D');
   dataloader->AddVariable("Wp_mass", "Wp_mass","",'D');
   dataloader->AddVariable("Phi_HW", "Phi_HW","",'D');
   dataloader->AddVariable("Eta_HW", "Eta_HW","",'D');
   dataloader->AddVariable("H_pT/mass_VH",   "H_pT/mass_VH","",'D');
   dataloader->AddVariable("Wp_pT/mass_VH",   "Wp_pT/mass_VH","",'D');
   dataloader->AddVariable("cosThetaStar", "cosThetaStar","",'D');
   dataloader->AddVariable("cosAlpha", "cosAlpha","",'D');
   dataloader->AddVariable("cosPhi", "cosPhi","",'D');
   */

   std::cout<<"Going to read input tree"<<std::endl;
   TTree *InTree = fChain->CloneTree(0);

   std::cout<<"Going to start Training !!!"<<std::endl;
   float signalWeight     = 1.0;
   float backgroundWeight = 1.0;

   dataloader->AddSignalTree    (fChain,     signalWeight);
   dataloader->AddBackgroundTree(fChain,     backgroundWeight);
   ///TCut mySigDef = "is_Signal == 1 &&  nJet > 4 && nBJet > 1";
   ///TCut myBkgDef = "is_Signal == 0 &&  nJet > 4 && nBJet > 1";

   TCut mySigDef = "is_Signal == 1 && H_mass < 250. && Wp_mass < 160";   
   TCut myBkgDef = "is_Signal == 0 && H_mass < 250. && Wp_mass < 160";

   dataloader->PrepareTrainingAndTestTree(mySigDef, myBkgDef, "SplitMode=Random:NormMode=NumEvents:!V:nTrain_Signal=0:nTest_Signal=0:nTrain_Background=100000:nTest_Background=100000");
   factory->BookMethod(dataloader,TMVA::Types::kBDT, "WpH_Tagger_v2","!H:!V:NTrees=600:MinNodeSize=2.0%:nCuts=40:BoostType=Grad:Shrinkage=0.1:UseBaggedBoost:GradBaggingFraction=0.25:SeparationType=GiniIndex:DoBoostMonitor:MaxDepth=5:NegWeightTreatment=IgnoreNegWeightsInTraining");
   factory->TrainAllMethods();
   factory->TestAllMethods();
   factory->EvaluateAllMethods();
   std::cout<<"Finished Training !!!"<<std::endl;
   outputFile->Close();
   
}


// This is the function we are aiming to call!
void TrainMVAEventBoosted(TChain *fChain){
   TFile *outputFile            = TFile::Open("output/TMVAResults_BOOSTED.root", "RECREATE" );
   TMVA::Factory *factory	= new TMVA::Factory("TMVAClassificationCategory", outputFile, "!V:!Silent:Transformations=I;P;G");
   TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

    // Adding variables:
   dataloader -> AddVariable("qtagj1", "qtagj1", "", 'D');
   dataloader -> AddVariable("qtagj2", "qtagj2", "", 'D');
   dataloader -> AddVariable("gtagj1", "gtagj1", "", 'D');
   dataloader -> AddVariable("gtagj2", "gtagj2", "", 'D');
   dataloader -> AddVariable("jj_m", "jj_m", "", 'D');
   dataloader -> AddVariable("pj1", "pj1", "", 'D');
   dataloader -> AddVariable("pj2", "pj2", "", 'D');
   dataloader -> AddVariable("ej1", "ej1", "", 'D');
   dataloader -> AddVariable("ej2", "ej2", "", 'D');
   dataloader -> AddVariable("nchadj1", "nchadj1", "", 'D');
   dataloader -> AddVariable("nchadj2", "nchadj2", "", 'D');
   dataloader -> AddVariable("nconstj1", "nconstj1", "", 'D');
   dataloader -> AddVariable("nconstj2", "nconstj2", "", 'D');

   /*
   m_myTree_Event->Branch("EventWeight",           &EventWeight); /// comment in once fixed !!!!!
   m_myTree_Event->Branch("nFJets",                &m_nFJets);
   m_myTree_Event->Branch("nTags",                 &m_NTags);
   m_myTree_Event->Branch("RWpTM_qqbb",            &m_RWpTM_qqbb);
   m_myTree_Event->Branch("RHpTM_qqbb",            &m_RHpTM_qqbb);
   m_myTree_Event->Branch("dPhi_WH_qqbb",          &m_dPhi_WH_qqbb);
   */

   TTree *InTree = fChain->CloneTree(0);

   std::cout<<"Going to start Training !!!"<<std::endl;
   float signalWeight     = 1.0;
   float backgroundWeight = 1.0;

   dataloader->AddSignalTree    (fChain,     signalWeight);
   dataloader->AddBackgroundTree(fChain,     backgroundWeight);

   TCut mySigDef = "is_Signal == 1 && nJets >= 1 && nFJets >= 2 && m_H > 90. && m_H < 140.";   // May need to alter here!
   TCut myBkgDef = "is_Signal == 3 && nJets >= 1 && nFJets >= 2 && m_H > 90. && m_H < 140.";

   dataloader->PrepareTrainingAndTestTree(mySigDef, myBkgDef, "SplitMode=Random:NormMode=NumEvents:!V:nTrain_Signal=0:nTest_Signal=0:nTrain_Background=0:nTest_Background=0");
   factory->BookMethod(dataloader,TMVA::Types::kBDT, "BoostedHpToWh_EventTagger_BOOSTED_qqbb","!H:!V:NTrees=600:MinNodeSize=2.0%:nCuts=40:BoostType=Grad:Shrinkage=0.1:UseBaggedBoost:GradBaggingFraction=0.25:SeparationType=GiniIndex:DoBoostMonitor:MaxDepth=4:NegWeightTreatment=IgnoreNegWeightsInTraining");
   factory->TrainAllMethods();
   factory->TestAllMethods();
   factory->EvaluateAllMethods();
   std::cout<<"Finished Training !!!"<<std::endl;
   outputFile->Close();
}

void TrainMVAEvent(TChain *fChain){
   TFile *outputFile            = TFile::Open("output/TMVAResults_v2.root", "RECREATE" );
   TMVA::Factory *factory	= new TMVA::Factory("TMVAClassificationCategory", outputFile, "!V:!Silent:Transformations=I;P;G");
   TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

   dataloader->AddVariable("Lep_pT", "Lep_pT","",'D');
   dataloader->AddVariable("MET", "MET","",'D');
   dataloader->AddVariable("HT6j", "HT6j", "",'D');
   dataloader->AddVariable("dR_lj_min", "dR_lj_min", "",'D');
   dataloader->AddVariable("dRbb_MindR_85", "dRbb_MindR_85", "",'D');
   dataloader->AddVariable("Mbj_MaxPt_85",          "Mbj_MaxPt_85","",'D');
   dataloader->AddVariable("Mbj_Wmass_85",          "Mbj_Wmass_85","",'D');
   dataloader->AddVariable("pT_jet1",               "pT_jet1","",'D');
   dataloader->AddVariable("pT_jet3",               "pT_jet3","",'D');
   dataloader->AddVariable("pT_jet5",               "pT_jet5","",'D');
   dataloader->AddVariable("pTbb_MindR_85",         "pTbb_MindR_85","",'D');
   dataloader->AddVariable("Muu_MindR_85",         "Muu_MindR_85","",'D');
   dataloader->AddVariable("pTuu_MindR_85",         "pTuu_MindR_85","",'D');
   dataloader->AddVariable("Mbb_MaxM_85",           "Mbb_MaxM_85","",'D');
   dataloader->AddVariable("dRlepbb_MindR_85",      "dRlepbb_MindR_85","",'D');
   dataloader->AddVariable("min_dPhi_vj",           "min_dPhi_vj","",'D');
   dataloader->AddVariable("Mlvj_MinPt",            "Mlvj_MinPt","",'D');
   dataloader->AddVariable("dRuu_MindR_85",          "dRuu_MindR_85","",'D');
   dataloader->AddVariable("dPhi_jjlv",              "dPhi_jjlv","",'D');
   dataloader->AddVariable("Mjj_MaxPt",              "Mjj_MaxPt","",'D');
   dataloader->AddVariable("Mjj_MinM",              "Mjj_MinM","",'D');
   dataloader->AddVariable("H2_jets",               "H2_jets","",'D');

   TTree *InTree = fChain->CloneTree(0);

   std::cout<<"Going to start Training !!!"<<std::endl;
   float signalWeight     = 1.0;
   float backgroundWeight = 1.0;

   dataloader->AddSignalTree    (fChain,     signalWeight);
   dataloader->AddBackgroundTree(fChain,     backgroundWeight);

   TCut mySigDef = "is_Signal == 1";
   ///TCut mySigDef = "is_Signal == 2";
   ///TCut myBkgDef = "is_Signal == 1";
   TCut myBkgDef = "is_Signal == 2";
   
   ///dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );
   ///dataloader->PrepareTrainingAndTestTree(mySigDef, myBkgDef, "SplitMode=Random:NormMode=NumEvents:!V:nTrain_Signal=0:nTest_Signal=0:nTrain_Background=200000:nTest_Background=200000");
   dataloader->PrepareTrainingAndTestTree(mySigDef, myBkgDef, "SplitMode=Random:NormMode=NumEvents:!V:nTrain_Signal=0:nTest_Signal=0:nTrain_Background=0:nTest_Background=0");
   factory->BookMethod(dataloader,TMVA::Types::kBDT, "WpH_Tagger_v2","!H:!V:NTrees=600:MinNodeSize=2.0%:nCuts=40:BoostType=Grad:Shrinkage=0.1:UseBaggedBoost:GradBaggingFraction=0.25:SeparationType=GiniIndex:DoBoostMonitor:MaxDepth=5:NegWeightTreatment=IgnoreNegWeightsInTraining");
   factory->TrainAllMethods();
   factory->TestAllMethods();
   factory->EvaluateAllMethods();
   std::cout<<"Finished Training !!!"<<std::endl;
   outputFile->Close();
}


std::vector<TTree*> GetTrees(TChain *fChain){

    std::vector<TTree*> trees;
    TTree    *Tree1 = fChain->CloneTree(0);
              Tree1->SetName("lvbb");
    TTree    *Tree2 = fChain->CloneTree(0);
              Tree1->SetName("qqbb");
    TTree    *Tree3 = fChain->CloneTree(0);
              Tree3->SetName("background");

    int      is_Signal;
    fChain->SetBranchAddress("is_Signal", &is_Signal);
    int count = 0;
    for (Long64_t jentry=0; jentry<fChain->GetEntriesFast(); jentry++) {
        fChain->GetEntry(jentry);
        if(is_Signal == 1)Tree1->Fill();
        else if(is_Signal == 2)Tree2->Fill();
        else if(is_Signal == 3)Tree3->Fill();
    }
    Tree1->Print();
    Tree2->Print();
    Tree3->Print();
    trees.push_back(Tree1);
    trees.push_back(Tree2);
    trees.push_back(Tree3);
    return trees;
}


void TrainMultiClassMVA(TChain *fChain){
   TFile *outputFile            = TFile::Open("output/TMVAResults_v3.root", "RECREATE" );
   TMVA::Factory *factory       = new TMVA::Factory( "TMVAMulticlass", outputFile,"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=multiclass" );
   TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");
   std::vector<TTree*> trees    = GetTrees(fChain);

   ///dataloader->AddVariable("Lep_pT", "Lep_pT","",'D');
   ///dataloader->AddVariable("MET", "MET","",'D');
   ///dataloader->AddVariable("HT6j", "HT6j", "",'D');
   ///dataloader->AddVariable("dR_lj_min", "dR_lj_min", "",'D');
   ///dataloader->AddVariable("dRbb_HiggsMass_85", "dRbb_HiggsMass_85", "",'D');
   ///dataloader->AddVariable("dRbb_MindR_85", "dRbb_MindR_85", "",'D');
   ///dataloader->AddVariable("Mbj_MaxPt_85",          "Mbj_MaxPt_85","",'D');
   ///dataloader->AddVariable("pT_jet1",               "pT_jet1","",'D');
   ///dataloader->AddVariable("pT_jet3",               "pT_jet3","",'D');
   ///dataloader->AddVariable("pT_jet5",               "pT_jet5","",'D');
   dataloader->AddVariable("pT_jet3/pT_jet1", "","",'D');
   dataloader->AddVariable("(Lep_pT+MET)/(Lep_pT+MET+HT6j)", "Lep_frac","",'D');
   dataloader->AddVariable("(HT6j)/(Lep_pT+MET+HT6j)", "Had_frac","",'D');
   ///dataloader->AddVariable("pTbb_MindR_85",         "pTbb_MindR_85","",'D');
   ///dataloader->AddVariable("pTuu_MindR_85",         "pTuu_MindR_85","",'D');
   dataloader->AddVariable("Mbb_MaxM_85",           "Mbb_MaxM_85","",'D');
   dataloader->AddVariable("dRlepbb_MindR_85",      "dRlepbb_MindR_85","",'D');
   ///dataloader->AddVariable("min_dPhi_vj",           "min_dPhi_vj","",'D');
   dataloader->AddVariable("Mlvj_MinPt",            "Mlvj_MinPt","",'D');
   ///dataloader->AddVariable("dRuu_MindR_70",          "dRuu_MindR_70","",'D');
   dataloader->AddVariable("dPhi_jjlv",              "dPhi_jjlv","",'D');

   TTree *signalTree  = trees.at(2);
   TTree *background0 = trees.at(1);
   TTree *background1 = trees.at(0);

   dataloader->AddTree    (signalTree,"Bkg");
   dataloader->AddTree    (background0,"lvbb");
   dataloader->AddTree    (background1,"qqbb");
   dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );
   factory->BookMethod(dataloader,TMVA::Types::kBDT, "WpH_Tagger_v2","!H:!V:NTrees=600:MinNodeSize=2.0%:nCuts=40:BoostType=Grad:Shrinkage=0.1:UseBaggedBoost:GradBaggingFraction=0.25:SeparationType=GiniIndex:DoBoostMonitor:MaxDepth=5:NegWeightTreatment=IgnoreNegWeightsInTraining");

   // Train MVAs using the set of training events
   factory->TrainAllMethods();
   // Evaluate all MVAs using the set of test events
   factory->TestAllMethods();
   // Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();
   // --------------------------------------------------------------
   // Save the output
   outputFile->Close();
   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;
   delete factory;
   delete dataloader;
}

void Evaluate_MultiClassMVA(TChain *fChain){

   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );
   
   float Lep_pT,MET,HT6j,dR_lj_min,dRbb_HiggsMass_85,dRbb_MindR_85,Mbj_MaxPt_85,pT_jet1,pT_jet3,pT_jet5,pTbb_MindR_85,pTuu_MindR_85,Mbb_MaxM_85,dRlepbb_MindR_85,min_dPhi_vj,Mlvj_MinPt,dRuu_MindR_70,dPhi_jjlv;

   reader->AddVariable("Lep_pT",                &Lep_pT);
   reader->AddVariable("MET",                   &MET);
   reader->AddVariable("HT6j",                  &HT6j);
   reader->AddVariable("dR_lj_min",             &dR_lj_min);
   reader->AddVariable("dRbb_HiggsMass_85",     &dRbb_HiggsMass_85);
   reader->AddVariable("dRbb_MindR_85",         &dRbb_MindR_85);
   reader->AddVariable("Mbj_MaxPt_85",          &Mbj_MaxPt_85);
   reader->AddVariable("pT_jet1",               &pT_jet1);
   reader->AddVariable("pT_jet3",               &pT_jet3);
   reader->AddVariable("pT_jet5",               &pT_jet5);
   reader->AddVariable("pTbb_MindR_85",         &pTbb_MindR_85);
   reader->AddVariable("pTuu_MindR_85",         &pTuu_MindR_85);
   reader->AddVariable("Mbb_MaxM_85",           &Mbb_MaxM_85);
   reader->AddVariable("dRlepbb_MindR_85",      &dRlepbb_MindR_85);
   reader->AddVariable("min_dPhi_vj",           &min_dPhi_vj);
   reader->AddVariable("Mlvj_MinPt",            &Mlvj_MinPt);
   reader->AddVariable("dRuu_MindR_70",         &dRuu_MindR_70);
   reader->AddVariable("dPhi_jjlv",             &dPhi_jjlv);
   reader->BookMVA("WpH_Tagger_v2",  "dataset/weights/TMVAMulticlass_WpH_Tagger_v2.weights.xml");
 
   // book output histograms
   UInt_t nbin = 60;

   TH1F *h_lvbb_r1  = new TH1F( "h_lvbb_r1", "", nbin, -7, 6 );
   TH1F *h_lvbb_r2  = new TH1F( "h_lvbb_r2", "", nbin, -7, 6 );
   TH1F *h_qqbb_r1  = new TH1F( "h_qqbb_r1", "", nbin, -7, 6 );
   TH1F *h_qqbb_r2  = new TH1F( "h_qqbb_r2", "", nbin, -7, 6 );
   TH1F *h_bkg_r1   = new TH1F( "h_bkg_r1", "", nbin, -7, 6 );
   TH1F *h_bkg_r2   = new TH1F( "h_bkg_r2", "", nbin, -7, 6 ); 
   TH2F *h_lvbb_r1_vs_r2 = new TH2F( "h_lvbb_r1_vs_r2", "", nbin, -7, 6 ,nbin,-7,6);
   TH2F	*h_qqbb_r1_vs_r2 = new TH2F( "h_qqbb_r1_vs_r2", "", nbin, -7, 6 ,nbin,-7,6);
   TH2F	*h_bkg_r1_vs_r2  = new TH2F( "h_bkg_r1_vs_r2", "", nbin, -7, 6 ,nbin,-7,6);
   TH1F *h_lvbb_ylvbb = new TH1F( "h_lvbb_ylvbb", "", nbin, 0, 1 );
   TH1F *h_lvbb_yqqbb = new TH1F( "h_lvbb_yqqbb", "", nbin, 0, 1 );
   TH1F *h_lvbb_ybkg  = new TH1F( "h_lvbb_ybkg", "",  nbin, 0, 1 );
   TH1F *h_qqbb_ylvbb = new TH1F( "h_qqbb_ylvbb", "", nbin, 0, 1 );
   TH1F *h_qqbb_yqqbb = new TH1F( "h_qqbb_yqqbb", "", nbin, 0, 1 );
   TH1F *h_qqbb_ybkg  = new TH1F( "h_qqbb_ybkg", "",  nbin, 0, 1 );
   TH1F *h_bkg_ylvbb  = new TH1F( "h_bkg_ylvbb", "",  nbin, 0, 1 );
   TH1F *h_bkg_yqqbb  = new TH1F( "h_bkg_yqqbb", "",  nbin, 0, 1 );
   TH1F *h_bkg_ybkg   = new TH1F( "h_bkg_ybkg", "",   nbin, 0, 1 );
   TH3F *h_lvbb_probabilities = new TH3F( "h_lvbb_probabilities", "", 50, 0, 1 ,50,0,1,50,0,1);
   TH3F *h_qqbb_probabilities = new TH3F( "h_qqbb_probabilities", "", 50, 0, 1 ,50,0,1,50,0,1);
   TH3F *h_bkg_probabilities  = new TH3F( "h_bkg_probabilities", "", 50, 0, 1 ,50,0,1,50,0,1);
   TH1F *h_lepFrac_lvbb  = new TH1F( "h_lepFrac_lvbb", "",  nbin, 0, 1 );
   TH1F *h_lepFrac_qqbb  = new TH1F( "h_lepFrac_qqbb", "",  nbin, 0, 1 );
   TH1F *h_lepFrac_bkg   = new TH1F( "h_lepFrac_bkg", "",   nbin, 0, 1 );
   TH1F *h_hadFrac_lvbb  = new TH1F( "h_hadfrac_lvbb", "",  nbin, 0, 1 );
   TH1F *h_HadFrac_qqbb  = new TH1F( "h_hadfrac_qqbb", "",  nbin, 0, 1 );
   TH1F *h_HadFrac_bkg   = new TH1F( "h_hadfrac_bkg", "",   nbin, 0, 1 );

   int  is_Signal;
   fChain->SetBranchAddress("is_Signal", &is_Signal);
   fChain->SetBranchAddress("Lep_pT",                &Lep_pT);
   fChain->SetBranchAddress("MET",                   &MET);
   fChain->SetBranchAddress("HT6j",                  &HT6j);
   fChain->SetBranchAddress("dR_lj_min",             &dR_lj_min);
   fChain->SetBranchAddress("dRbb_HiggsMass_85",     &dRbb_HiggsMass_85);
   fChain->SetBranchAddress("dRbb_MindR_85",         &dRbb_MindR_85);
   fChain->SetBranchAddress("Mbj_MaxPt_85",          &Mbj_MaxPt_85);
   fChain->SetBranchAddress("pT_jet1",               &pT_jet1);
   fChain->SetBranchAddress("pT_jet3",               &pT_jet3);
   fChain->SetBranchAddress("pT_jet5",               &pT_jet5);
   fChain->SetBranchAddress("pTbb_MindR_85",         &pTbb_MindR_85);
   fChain->SetBranchAddress("pTuu_MindR_85",         &pTuu_MindR_85);
   fChain->SetBranchAddress("Mbb_MaxM_85",           &Mbb_MaxM_85);
   fChain->SetBranchAddress("dRlepbb_MindR_85",	&dRlepbb_MindR_85);
   fChain->SetBranchAddress("min_dPhi_vj",           &min_dPhi_vj);
   fChain->SetBranchAddress("Mlvj_MinPt",            &Mlvj_MinPt);
   fChain->SetBranchAddress("dRuu_MindR_70",         &dRuu_MindR_70);
   fChain->SetBranchAddress("dPhi_jjlv",             &dPhi_jjlv);

   float n_lvbb_SR1 = 0.;
   float n_lvbb_SR2 = 0.;
   float n_lvbb_CR  = 0.;
   float n_lvbb_VR  = 0.;
   float n_qqbb_SR1 = 0.;
   float n_qqbb_SR2 = 0.;
   float n_qqbb_CR  = 0.;
   float n_qqbb_VR  = 0.;
   float n_bkg_SR1 = 0.;
   float n_bkg_SR2 = 0.;
   float n_bkg_CR  = 0.;
   float n_bkg_VR  = 0.;

   for (Long64_t ievt=0; ievt<fChain->GetEntries();ievt++) {
      if (ievt%1000 == 0){
         std::cout << "--- ... Processing event: " << ievt << std::endl;
      }
      fChain->GetEntry(ievt);
 
      float p_bkg  = reader->EvaluateMulticlass("WpH_Tagger_v2")[0];
      float p_lvbb = reader->EvaluateMulticlass("WpH_Tagger_v2")[1];
      float p_qqbb = reader->EvaluateMulticlass("WpH_Tagger_v2")[2]; 
         
      bool passed_SR1  = p_lvbb > 0.4  && p_qqbb < 0.25 && p_bkg < 0.6;
      bool passed_SR2  = p_qqbb > 0.25 && p_lvbb < 0.4  && p_bkg < 0.6;   
      bool passed_CR   = p_bkg  > 0.6;
      bool passed_VR   = p_bkg  < 0.6  && p_lvbb < 0.4 && p_qqbb < 0.25;

      float y_max  = std::max(p_bkg,std::max(p_lvbb, p_qqbb));
      if(is_Signal==2){
          h_lvbb_r1->Fill(log(p_lvbb/p_bkg));
          h_lvbb_r2->Fill(log(p_qqbb/p_bkg));
          h_lvbb_r1_vs_r2->Fill(log(p_lvbb/p_bkg),log(p_qqbb/p_bkg));
          if(p_lvbb > p_qqbb && p_lvbb > p_bkg)h_lvbb_ylvbb->Fill(y_max);
          if(p_qqbb > p_lvbb && p_qqbb > p_bkg)h_lvbb_yqqbb->Fill(y_max);
          if(p_bkg  > p_lvbb && p_bkg  > p_qqbb)h_lvbb_ybkg->Fill(y_max);          
          h_lvbb_probabilities->Fill(p_lvbb,p_qqbb,p_bkg);
          if(passed_SR1)n_lvbb_SR1++;
       	  if(passed_SR2)n_lvbb_SR2++;
       	  if(passed_CR)n_lvbb_CR++;
          if(passed_VR)n_lvbb_VR++;
      }else if(is_Signal == 1){
          h_qqbb_r1->Fill(log(p_lvbb/p_bkg));
          h_qqbb_r2->Fill(log(p_qqbb/p_bkg));
          h_qqbb_r1_vs_r2->Fill(log(p_lvbb/p_bkg),log(p_qqbb/p_bkg));
          if(p_lvbb > p_qqbb && p_lvbb > p_bkg)h_qqbb_ylvbb->Fill(y_max);
          if(p_qqbb > p_lvbb && p_qqbb > p_bkg)h_qqbb_yqqbb->Fill(y_max);
          if(p_bkg  > p_lvbb && p_bkg  > p_qqbb)h_qqbb_ybkg->Fill(y_max);
          h_qqbb_probabilities->Fill(p_lvbb,p_qqbb,p_bkg);
       	  if(passed_SR1)n_qqbb_SR1++;
          if(passed_SR2)n_qqbb_SR2++;
          if(passed_CR)n_qqbb_CR++;
          if(passed_VR)n_qqbb_VR++;
      }else if(is_Signal == 3){
          h_bkg_r1->Fill(log(p_lvbb/p_bkg));
          h_bkg_r2->Fill(log(p_qqbb/p_bkg));
          h_bkg_r1_vs_r2->Fill(log(p_lvbb/p_bkg),log(p_qqbb/p_bkg));
          if(p_lvbb > p_qqbb && p_lvbb > p_bkg)h_bkg_ylvbb->Fill(y_max);
          if(p_qqbb > p_lvbb && p_qqbb > p_bkg)h_bkg_yqqbb->Fill(y_max);
          if(p_bkg  > p_lvbb && p_bkg  > p_qqbb)h_bkg_ybkg->Fill(y_max);
          h_bkg_probabilities->Fill(p_lvbb,p_qqbb,p_bkg);
       	  if(passed_SR1)n_bkg_SR1++;
          if(passed_SR2)n_bkg_SR2++;
          if(passed_CR)n_bkg_CR++;
          if(passed_VR)n_bkg_VR++;
      }
   }
     std::cout<<"n_lvbb_SR1:  "<<n_lvbb_SR1 <<std::endl;
     std::cout<<"n_lvbb_SR2:  "<<n_lvbb_SR2 <<std::endl;
     std::cout<<"n_lvbb_CR:   "<<n_lvbb_CR <<std::endl;
     std::cout<<"n_lvbb_VR:   "<<n_lvbb_VR <<std::endl;
     std::cout<<"n_qqbb_SR1:  "<<n_qqbb_SR1 <<std::endl;
     std::cout<<"n_qqbb_SR2:  "<<n_qqbb_SR2 <<std::endl;
     std::cout<<"n_qqbb_CR:   "<<n_qqbb_CR <<std::endl;
     std::cout<<"n_qqbb_VR:   "<<n_qqbb_VR <<std::endl;
     std::cout<<"n_bkg_SR1:   "<<n_bkg_SR1 <<std::endl;
     std::cout<<"n_bkg_SR2:   "<<n_bkg_SR2 <<std::endl;
     std::cout<<"n_bkg_CR:    "<<n_bkg_CR  <<std::endl;
     std::cout<<"n_bkg_VR:    "<<n_bkg_VR  <<std::endl;

     int n_events = h_lvbb_r1->Integral(-1,h_lvbb_r1->GetNbinsX()+1);
     h_lvbb_r1->Scale(1./n_events);
     h_lvbb_r1->SetLineColor(kBlue);
     n_events = h_lvbb_r2->Integral(-1,h_lvbb_r2->GetNbinsX()+1);
     h_lvbb_r2->Scale(1./n_events);
     h_lvbb_r2->SetLineColor(kBlue);
     n_events = h_qqbb_r1->Integral(-1,h_qqbb_r1->GetNbinsX()+1);
     h_qqbb_r1->Scale(1./n_events);
     h_qqbb_r1->SetLineColor(kRed);
     n_events = h_qqbb_r2->Integral(-1,h_qqbb_r2->GetNbinsX()+1);
     h_qqbb_r2->Scale(1./n_events);
     h_lvbb_r2->SetLineColor(kRed);
     n_events = h_bkg_r1->Integral(-1,h_bkg_r1->GetNbinsX()+1);
     h_bkg_r1->Scale(1./n_events);
     h_bkg_r1->SetLineColor(kBlack);
     n_events = h_bkg_r2->Integral(-1,h_bkg_r2->GetNbinsX()+1);
     h_bkg_r2->Scale(1./n_events);
     h_bkg_r2->SetLineColor(kBlack);

     n_events = h_lvbb_ylvbb->Integral(-1,h_lvbb_ylvbb->GetNbinsX()+1);
     h_lvbb_ylvbb->Scale(1./n_events);
     h_lvbb_ylvbb->SetLineColor(kBlue);
     std::cout<<" h_lvbb_ylvbb "<<n_events<<std::endl;
     n_events = h_lvbb_yqqbb->Integral(-1,h_lvbb_yqqbb->GetNbinsX()+1);
     h_lvbb_yqqbb->Scale(1./n_events);
     h_lvbb_yqqbb->SetLineColor(kBlue);
     std::cout<<" h_lvbb_yqqbb "<<n_events<<std::endl;
     n_events = h_lvbb_ybkg->Integral(-1,h_qqbb_ybkg->GetNbinsX()+1);
     h_lvbb_ybkg->Scale(1./n_events);
     h_lvbb_ybkg->SetLineColor(kBlue);
     std::cout<<" h_lvbb_ybkg "<<n_events<<std::endl;

     n_events = h_qqbb_ylvbb->Integral(-1,h_qqbb_ylvbb->GetNbinsX()+1);
     h_qqbb_ylvbb->Scale(1./n_events);
     h_qqbb_ylvbb->SetLineColor(kRed);
     std::cout<<" h_qqbb_ylvbb "<<n_events<<std::endl;
     n_events = h_qqbb_yqqbb->Integral(-1,h_qqbb_yqqbb->GetNbinsX()+1);
     h_qqbb_yqqbb->Scale(1./n_events);
     h_qqbb_yqqbb->SetLineColor(kRed);
     std::cout<<" h_qqbb_yqqbb "<<n_events<<std::endl;
     n_events = h_qqbb_ybkg->Integral(-1,h_qqbb_ybkg->GetNbinsX()+1);
     h_qqbb_ybkg->Scale(1./n_events);
     h_qqbb_ybkg->SetLineColor(kRed);
     std::cout<<" h_qqbb_ybkg "<<n_events<<std::endl;

     n_events = h_bkg_ylvbb->Integral(-1,h_bkg_ylvbb->GetNbinsX()+1);
     h_bkg_ylvbb->Scale(1./n_events);
     h_bkg_ylvbb->SetLineColor(kBlack);
     std::cout<<" h_bkg_ylvbb "<<n_events<<std::endl;
     n_events = h_bkg_yqqbb->Integral(-1,h_bkg_yqqbb->GetNbinsX()+1);
     h_bkg_yqqbb->Scale(1./n_events);
     h_bkg_yqqbb->SetLineColor(kBlack);
     std::cout<<" h_bkg_yqqbb "<<n_events<<std::endl;
     n_events = h_bkg_ybkg->Integral(-1,h_bkg_ybkg->GetNbinsX()+1);
     h_bkg_ybkg->Scale(1./n_events);
     h_bkg_ybkg->SetLineColor(kBlack);
     std::cout<<" h_bkg_ybkg "<<n_events<<std::endl;

     TCanvas *c1 = new TCanvas("canvas1","",720,720);
     c1->cd();
     h_lvbb_r1->Draw("HIST");
     h_qqbb_r1->Draw("HISTSAME");
     h_bkg_r1->Draw("HISTSAME");
     c1->SaveAs("TEST1.pdf");

     TCanvas *c2 = new TCanvas("canvas2","",720,720);
     c2->cd();
     h_lvbb_r2->Draw("HIST");
     h_qqbb_r2->Draw("HISTSAME");
     h_bkg_r2->Draw("HISTSAME");
     c2->SaveAs("TEST2.pdf");

     TCanvas *c3 = new TCanvas("canvas3","",720,720);
     c3->cd();
     h_lvbb_r1_vs_r2->Draw("COLZ");
     c3->SaveAs("TEST3.pdf");

     TCanvas *c4 = new TCanvas("canvas4","",720,720);
     c4->cd();
     h_qqbb_r1_vs_r2->Draw("COLZ");
     c4->SaveAs("TEST4.pdf");

     TCanvas *c5 = new TCanvas("canvas5","",720,720);
     c5->cd();
     h_bkg_r1_vs_r2->Draw("COLZ");
     c5->SaveAs("TEST5.pdf");

     TCanvas *c6 = new TCanvas("canvas6","",720,720);
     c6->cd();
     h_lvbb_ylvbb->Draw("HIST");
     h_qqbb_ylvbb->Draw("HISTSAME");
     h_bkg_ylvbb->Draw("HISTSAME");
     c6->SaveAs("TEST6.pdf");

     TCanvas *c7 = new TCanvas("canvas7","",720,720);
     c7->cd();
     h_lvbb_yqqbb->Draw("HIST");
     h_qqbb_yqqbb->Draw("HISTSAME");
     h_bkg_yqqbb->Draw("HISTSAME");
     c7->SaveAs("TEST7.pdf");

     TCanvas *c8 = new TCanvas("canvas8","",720,720);
     c8->cd();
     h_lvbb_ybkg->Draw("HIST");
     h_qqbb_ybkg->Draw("HISTSAME");
     h_bkg_ybkg->Draw("HISTSAME");
     c8->SaveAs("TEST8.pdf");

     TFile *outfile     = TFile::Open("PlotFiles.root", "RECREATE" );
     outfile->cd();
     h_lvbb_probabilities->Write();
     h_qqbb_probabilities->Write();
     h_bkg_probabilities->Write();
     h_lvbb_r1->Write();
     h_qqbb_r1->Write();
     h_bkg_r1->Write();
     h_lvbb_r2->Write();
     h_qqbb_r2->Write();
     h_bkg_r2->Write();
     h_lvbb_r1_vs_r2->Write();
     h_qqbb_r1_vs_r2->Write();
     h_bkg_r1_vs_r2->Write();
     h_lvbb_ylvbb->Write();
     h_qqbb_ylvbb->Write();
     h_bkg_ylvbb->Write();
     h_lvbb_yqqbb->Write();
     h_qqbb_yqqbb->Write();
     h_bkg_yqqbb->Write();
     h_lvbb_ybkg->Write();
     h_qqbb_ybkg->Write();
     h_bkg_ybkg->Write();
     outfile->Close();
}
