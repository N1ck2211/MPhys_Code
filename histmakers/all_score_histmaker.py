# list of processes (mandatory)
processList = {
    "wzp6_ee_qq_ecm125": {"fraction": 0.1},
}

# Production tag when running over EDM4Hep centrally produced events, this points to the yaml files for getting sample statistics (mandatory)
# prodTag = "FCCee/winter2023/IDEA/"

# Link to the dictionary that contains all the cross-section information etc... (mandatory)
procDict = "FCCee_procDict_winter2023_IDEA.json"

# Define the input dir (optional)
inputDir = "./outputs/treemaker/combined_jets"

# Optional: output directory, default is local running directory
outputDir = "./outputs/histmaker/Zgammaqq/no_cut"

# optional: ncpus, default is 4, -1 uses all cores available
nCPUS = -1

# scale the histograms with the cross-section and integrated luminosity
doScale = True
intLumi = 10000000  # 10 /ab

score_to_cut = 0.0

#cut_phrase = "scoresum_Q >" + str(score_to_cut)

# define some binning for various histograms
bins_p_mu = (2000, 0, 200)  # 100 MeV bins
bins_m_ll = (2000, 0, 200)  # 100 MeV bins
bins_p = (2000, -100, 100)
bins_p_ll = (2000, 0, 200)  # 100 MeV bins
bins_recoil = (200000, 0, 200)  # 1 MeV bins
bins_cosThetaMiss = (10000, 0, 1)

bins_m_jj = (100, 50, 150)  # 1 GeV bins
bins_score = (50, 0, 2.0)  #
bins_n  = (50, 0, 20000000)

bins_theta = (500, -5, 5)
bins_eta = (600, -3, 3)
bins_phi = (500, -1, 1)

bins_count = (50, 0, 50)
bins_charge = (9, -4.5, 4.5)
bins_iso = (500, 0, 5)
bins_e = (600, 0, 60)

# build_graph function that contains the analysis logic, cuts and histograms (mandatory)
def build_graph(df, dataset):

    results = []
    df = df.Define("weight", "1.0")
    weightsum = df.Sum("weight")

    #########
    ### CUT: cut on the jet tagging score to select H->bb events
    #########

    df = df.Define('Jet_One', 'recojet_isB[0]')
    df = df.Define('Jet_Two', 'recojet_isB[1]')

    df = df.Define("scoresum_B", "recojet_isB[0] + recojet_isB[1]")
    df = df.Define("scoresum_Q", "recojet_isQ[0] + recojet_isQ[1]")
    df = df.Define("scoresum_S", "recojet_isS[0] + recojet_isS[1]")
    df = df.Define("scoresum_C", "recojet_isC[0] + recojet_isC[1]")
    df = df.Define("scoresum_G", "recojet_isG[0] + recojet_isG[1]")

    df = df.Define('N_G', "scoresum_B * 10000000")
    
    # Create Histograms for individual jets
    results.append(df.Histo1D(('Jet_One', '', *bins_score), 'Jet_One'))
    results.append(df.Histo1D(('Jet_Two', '', *bins_score), 'Jet_Two'))

    results.append(df.Histo1D(("scoresum_B", "", *bins_score), "scoresum_B"))
    results.append(df.Histo1D(("scoresum_Q", "", *bins_score), "scoresum_Q"))
    results.append(df.Histo1D(("scoresum_S", "", *bins_score), "scoresum_S"))
    results.append(df.Histo1D(("scoresum_C", "", *bins_score), "scoresum_C"))
    results.append(df.Histo1D(("scoresum_G", "", *bins_score), "scoresum_G"))

    results.append(df.Histo1D(("N_G", "", *bins_n), "N_G"))

    #df = df.Filter(cut_phrase)

    results.append(df.Histo1D(("jj_p", "Number of Events of a Given Momentum, Selected via a cut of Gluon/Quark Tagging Score", *bins_p), "jj_p"))

    results.append(df.Histo1D(("jj_theta", "", *bins_theta), "jj_theta"))
    results.append(df.Histo1D(("jj_phi", "", *bins_phi), "jj_phi"))
    results.append(df.Histo1D(("jj_m", "", *bins_m_jj), "jj_m"))
    results.append(df.Histo1D(("jj_q", "", *bins_charge), "jj_q"))

    results.append(df.Histo1D(("jj_e", "", *bins_e), "jj_e"))

    return results, weightsum
