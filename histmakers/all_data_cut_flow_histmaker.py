# list of processes (mandatory)
processList = {
    "wzp6_ee_Hgg_ecm125": {"fraction": 1}, "wzp6_ee_qq_ecm125": {"fraction": 1},
}

# Production tag when running over EDM4Hep centrally produced events, this points to the yaml files for getting sample statistics (mandatory)
#prodTag = "FCCee/winter2023/IDEA/"

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

bins_count = (8, 0, 9)
bins_charge = (9, -4.5, 4.5)
bins_iso = (500, 0, 5)
bins_e = (600, 0, 60)

bins_flow = (100, 0, 100)

# build_graph function that contains the analysis logic, cuts and histograms (mandatory)
def build_graph(df, dataset):

    results = []
    df = df.Define("weight", "1.0")
    weightsum = df.Sum("weight")

    #########
    ### CUT 0: all events
    #########
    df = df.Define("cut0", "0")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "cut0"))

    #########
    ### CUT 1: Number Chad
    #########
    df = df.Filter("jet_nchad[0] > 10")
    df = df.Filter("jet_nchad[1] > 7")
    df = df.Define("CHad", "1")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "CHad"))

    #########
    ### CUT 2: Number NNhad
    ##########
   # df = df.Filter("jet_nnhad[0] > 12")
   # df = df.Filter("jet_nnhad[1] > 9")
   # df = df.Define("cut2", "2")
   # results.append(df.Histo1D(("cutFlow", "", *bins_flow), "cut2"))

    #########
    ### CUT 3: Number NConst
    #########
    df = df.Filter("jet_nconst[0] > 21")
    df = df.Filter("jet_nconst[1] > 14")
    df = df.Define("nconst", "2")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "nconst"))

    #########
    ### CUT 1: Momentum
    #########
    df = df.Filter("jj_p[0] > 38")
    df = df.Filter("jj_p[1] > 46")
    df = df.Define("jetp", "3")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "jetp"))

    #########
    ### CUT 2: Transverse Momentum
    #########
    df = df.Filter("jj_pt[0] > 45")
    df = df.Filter("jj_pt[1] > 45")
    df = df.Define("jetpt", "4")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "jetpt"))

    #########
    ### CUT 3: Energy
    #########
    df = df.Filter("jj_e[0] > 62")
    df = df.Filter("jj_e[1] > 50")
    df = df.Define("jete", "5")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "jete"))

    #########
    ### CUT 4: Strange Score
    #########
#    df = df.Filter("recojet_isS[0] < 0.1")
#    df = df.Filter("recojet_isS[1] < 0.1")
#    df = df.Define("stag", "6")
#    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "stag"))

    #########
    ### CUT 5: Light Score
    #########
    df = df.Filter("recojet_isQ[0] > 0.02")
    df = df.Filter("recojet_isQ[1] > 0.02")
    df = df.Define("qtag", "6")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "qtag"))

    #########
    ### CUT 6: Gluon Score
    #########
    df = df.Filter("recojet_isG[0] > 0.84")
    df = df.Filter("recojet_isG[1] > 0.85")
    df = df.Define("gtag", "7")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "gtag"))

    #########
    ### CUT 7: Individual Mass
    #########
    df = df.Filter("jet_m[0] > 21")
    df = df.Filter("jet_m[1] > 14")
    df = df.Define("ind_jet_mass", "8")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "ind_jet_mass"))

    #########
    ### CUT 7: Mass
    #########
    df = df.Filter("jj_m > 120")
    df = df.Define("jets_mass", "9")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "jets_mass"))

    #########
    ### CUT 8: Eta
    #########
    df = df.Filter("jj_eta[0] > -1.4")
    df = df.Filter("jj_eta[1] > -5.2")
    df = df.Define("jet_eta", "10")
    results.append(df.Histo1D(("cutFlow", "", *bins_flow), "jet_eta"))

    #################

    results.append(df.Histo1D(("jj_p", "", *bins_p), "jj_p"))

    results.append(df.Histo1D(("jj_theta", "", *bins_theta), "jj_theta"))
    results.append(df.Histo1D(("jj_phi", "", *bins_phi), "jj_phi"))
    results.append(df.Histo1D(("jj_m", "", *bins_m_jj), "jj_m"))
    results.append(df.Histo1D(("jj_eta", "", *bins_m_jj), "jj_eta"))

    results.append(df.Histo1D(("jj_e", "", *bins_e), "jj_e"))
    results.append(df.Histo1D(("jj_pt", "", *bins_p), "jj_pt"))

    return results, weightsum
