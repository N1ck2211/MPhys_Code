import os

# list of processes
processList = {
    "wzp6_ee_Hgg_ecm125": {"fraction": 0.1},
    "wzp6_ee_qq_ecm125": {"fraction": 0.1}
}

# Production tag when running over EDM4Hep centrally produced events, this points to the yaml files for getting
# sample statistics (mandatory)
# prodTag = "FCCee/winter2023/IDEA/"

# Optional: output directory, default is local running directory
outputDir = "./outputs/treemaker/clustered_jets/"

# Define the input dir (optional)
inputDir = "/eos/experiment/fcc/ee/generation/DelphesEvents/winter2023/IDEA/"

# additional/custom C++ functions, defined in header files (optional)
includePaths = ["functions.h"]

# latest particle transformer model, trained on 9M jets in winter2023 samples
model_name = "fccee_flavtagging_edm4hep_wc_v1"

# model files needed for unit testing in CI
url_model_dir = "https://fccsw.web.cern.ch/fccsw/testsamples/jet_flavour_tagging/winter2023/wc_pt_13_01_2022/"
url_preproc = "{}/{}.json".format(url_model_dir, model_name)
url_model = "{}/{}.onnx".format(url_model_dir, model_name)

# model files locally stored on /eos
model_dir = (
    "/eos/experiment/fcc/ee/jet_flavour_tagging/winter2023/wc_pt_13_01_2022/"
)
local_preproc = "{}/{}.json".format(model_dir, model_name)
local_model = "{}/{}.onnx".format(model_dir, model_name)


# get local file, else download from url
def get_file_path(url, filename):
    if os.path.exists(filename):
        return os.path.abspath(filename)
    else:
        urllib.request.urlretrieve(url, os.path.basename(url))
        return os.path.basename(url)


weaver_preproc = get_file_path(url_preproc, local_preproc)
weaver_model = get_file_path(url_model, local_model)

from addons.ONNXRuntime.python.jetFlavourHelper import JetFlavourHelper
from addons.FastJet.python.jetClusteringHelper import (ExclusiveJetClusteringHelper, )

jetFlavourHelper = None
jetClusteringHelper = None


# Mandatory: RDFanalysis class where the use defines the operations on the TTree
class RDFanalysis:

    # __________________________________________________________ Mandatory: analysers funtion to define the analysers
    # to process, please make sure you return the last dataframe, in this example it is df2
    def analysers(df):
        # __________________________________________________________ Mandatory: analysers funtion to define the
        # analysers to process, please make sure you return the last dataframe, in this example it is df2

        # define some aliases to be used later on
        df = df.Alias("Particle0", "Particle#0.index")
        df = df.Alias("Particle1", "Particle#1.index")
        df = df.Alias("MCRecoAssociations0", "MCRecoAssociations#0.index")
        df = df.Alias("MCRecoAssociations1", "MCRecoAssociations#1.index")

        # here cluster jets in the events but first remove muons from the list of reconstructed particles

        # perform N=2 jet clustering
        global jetClusteringHelper
        global jetFlavourHelper

        # define jet and run clustering parameters
        # name of collections in EDM root files
        collections = {"GenParticles": "Particle", "PFParticles": "ReconstructedParticles",
                       "PFTracks": "EFlowTrack", "PFPhotons": "EFlowPhoton", "PFNeutralHadrons": "EFlowNeutralHadron",
                       "TrackState": "EFlowTrack_1", "TrackerHits": "TrackerHits", "CalorimeterHits": "CalorimeterHits",
                       "dNdx": "EFlowTrack_2", "PathLength": "EFlowTrack_L", "Bz": "magFieldBz"}

        jetClusteringHelper = ExclusiveJetClusteringHelper(collections["PFParticles"], 2)
        df = jetClusteringHelper.define(df)

        # define jet flavour tagging parameters

        jetFlavourHelper = JetFlavourHelper(collections, jetClusteringHelper.jets, jetClusteringHelper.constituents, )

        # define observables for tagger
        df = jetFlavourHelper.define(df)

        # tagger inference
        df = jetFlavourHelper.inference(weaver_preproc, weaver_model, df)

        # CUT 1: Njets = 2
        df = df.Filter("event_njet > 1")

        df = df.Define("jets_p4", "JetConstituentsUtils::compute_tlv_jets({})".format(jetClusteringHelper.jets), )

       #df = df.Define("sum_jets_p4", "JetConstituentsUtils::sum_tlv_constituents({})".format(jetClusteringHelper.constituents), )

        df = df.Define("jj_m", "JetConstituentsUtils::InvariantMass(jets_p4[0], jets_p4[1])", )

        df = df.Define("jj_p", "JetClusteringUtils::get_p({})".format(jetClusteringHelper.jets), )
        df = df.Define("jj_theta", "JetClusteringUtils::get_theta({})".format(jetClusteringHelper.jets), )
        df = df.Define("jj_phi", "JetClusteringUtils::get_phi({})".format(jetClusteringHelper.jets), )
        # df = df.Define("jj_q", "JetClusteringUtils::get_charge({})[0]".format(jetClusteringHelper.constituents), )
        df = df.Define("jj_eta", "JetClusteringUtils::get_eta({})".format(jetClusteringHelper.jets), )

        df = df.Define("jj_e", "JetClusteringUtils::get_e({})".format(jetClusteringHelper.jets), )
        df = df.Define("jj_pt", "JetClusteringUtils::get_pt({})".format(jetClusteringHelper.jets), )

        return df

    # Mandatory: output function, please make sure you return the branch-list as a python list
    def output():
        branchList = ["jj_m", "jj_p", "jj_theta", "jj_phi", "jj_e", "jj_pt", "jj_eta"]

        # outputs jet properties
        branchList += jetClusteringHelper.outputBranches()

        # outputs jet scores and constituent breakdown
        branchList += jetFlavourHelper.outputBranches()

        return branchList
