# list of processes (mandatory)
processList = {
    "wzp6_ee_Hgg_ecm125_1": {"fraction": 1}, "wzp6_ee_qq_ecm125": {"fraction": 1},
}

# Production tag when running over EDM4Hep centrally produced events, this points to the yaml files for getting sample statistics (mandatory)
# prodTag = "FCCee/winter2023/IDEA/"

# Link to the dictionary that contains all the cross-section information etc... (mandatory)
procDict = "FCCee_procDict_winter2023_IDEA.json"

# Define the input dir (optional)
inputDir = "./full_clustering_output/treemaker"

# Optional: output directory, default is local running directory
outputDir = "./full_clustering_output/histmaker"

# optional: ncpus, default is 4, -1 uses all cores available
nCPUS = -1

# scale the histograms with the cross-section and integrated luminosity
doScale = True
intLumi = 10000000  # 10 /ab

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

    df = df.Define("scoresum_B", "recojet_isB[0] + recojet_isB[1]")
    df = df.Define("scoresum_Q", "recojet_isQ[0] + recojet_isQ[1]")
    df = df.Define("scoresum_S", "recojet_isS[0] + recojet_isS[1]")
    df = df.Define("scoresum_C", "recojet_isC[0] + recojet_isC[1]")
    df = df.Define("scoresum_G", "recojet_isG[0] + recojet_isG[1]")
    
    # Create Histograms for individual jets
    results.append(df.Histo1D(("scoresum_B", "", *bins_score), "scoresum_B"))
    results.append(df.Histo1D(("scoresum_Q", "", *bins_score), "scoresum_Q"))
    results.append(df.Histo1D(("scoresum_S", "", *bins_score), "scoresum_S"))
    results.append(df.Histo1D(("scoresum_C", "", *bins_score), "scoresum_C"))
    results.append(df.Histo1D(("scoresum_G", "", *bins_score), "scoresum_G"))

    #########
    ### CUT 0: all events
    #########
    df = df.Define("cut0", "0")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut0"))


    #########
    ### CUT 1: Number Chad
    #########
    df = df.Filter("jet_nchad[0] > 14")
    df = df.Filter("jet_nchad[1] > 10")
    df = df.Define("cut1", "1")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut1"))

    #########
    ### CUT 2: Number NConst
    #########
    df = df.Filter("jet_nconst[0] > 30")
    df = df.Filter("jet_nconst[1] > 20")
    df = df.Define("cut2", "2")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut2"))

    #########
    ### CUT 3: Momentum
    #########
    df = df.Filter("jj_p[0] > 36")
    df = df.Filter("jj_p[1] > 40")
    df = df.Define("cut3", "3")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut3"))

    #########
    ### CUT 4: Energy
    #########
    df = df.Filter("jj_e[0] > 62")
    df = df.Filter("jj_p[1] > 48")
    df = df.Define("cut4", "4")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut4"))

    #########
    ### CUT 5: Strange Score
    #########
    df = df.Filter("recojet_isS[0] > 0.00")
    df = df.Filter("recojet_isS[1] > 0.02")
    df = df.Define("cut5", "5")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut5"))

    #########
    ### CUT 6: Light Score
    #########
    df = df.Filter("recojet_isQ[0] > 0.02")
    df = df.Filter("recojet_isQ[1] > 0.02")
    df = df.Define("cut6", "6")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut6"))

    #########
    ### CUT 7: Gluon Score
    #########
    df = df.Filter("recojet_isG[0] > 0.24")
    df = df.Filter("recojet_isG[1] > 0.22")
    df = df.Define("cut7", "7")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut7"))

    #########
    ### CUT 8: Mass
    #########
    df = df.Filter("jj_m > 115")
    df = df.Define("cut8", "8")
    results.append(df.Histo1D(("cutFlow", "", *bins_count), "cut8"))

    #################

    results.append(df.Histo1D(("jj_p", "", *bins_p), "jj_p"))

    results.append(df.Histo1D(("jj_theta", "", *bins_theta), "jj_theta"))
    results.append(df.Histo1D(("jj_phi", "", *bins_phi), "jj_phi"))
    results.append(df.Histo1D(("jj_m", "", *bins_m_jj), "jj_m"))
    results.append(df.Histo1D(("jj_eta", "", *bins_m_jj), "jj_eta"))

    results.append(df.Histo1D(("jj_e", "", *bins_e), "jj_e"))
    results.append(df.Histo1D(("jj_pt", "", *bins_p), "jj_pt"))

    return results, weightsum
