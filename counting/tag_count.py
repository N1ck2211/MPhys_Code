import numpy as np
# list of processes (mandatory)
processList = {
    "wzp6_ee_qq_ecm125": {"fraction": 0.1},
}

# Production tag when running over EDM4Hep centrally produced events, this points to the yaml files for getting sample statistics (mandatory)
# prodTag = "FCCee/winter2023/IDEA/"

# Link to the dictionary that contains all the cross-section information etc... (mandatory)
procDict = "FCCee_procDict_winter2023_IDEA.json"

# Define the input dir (optional)
inputDir = "./outputs/treemaker/clustered_jets"

# Optional: output directory, default is local running directory
outputDir = "./outputs/histmaker/Zgammaqq/counts"

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
bins_n = (50, 0, 20000000)

bins_theta = (500, -5, 5)
bins_eta = (600, -3, 3)
bins_phi = (500, -1, 1)

bins_count = (50, 0, 50)
bins_charge = (9, -4.5, 4.5)
bins_iso = (500, 0, 5)
bins_e = (600, 0, 60)

value_to_count = 'recojet_isG[0]'
name = 'background_jj_gg0'
# build_graph function that contains the analysis logic, cuts and histograms (mandatory)

def build_graph(df, dataset):
    results = []
    df = df.Define("weight", "1.0")
    weightsum = df.Sum("weight")

    df = df.Define('score', value_to_count)

    f = open((name + '.csv'), 'a')
    score_range = np.arange(0, 1, 0.02)

    for i in score_range:
        cut_phrase = "score >" + str(i)

        df = df.Filter(cut_phrase)
        hist = df.Histo1D(("recojet_isG", "", *bins_score), "recojet_isG")
        count = hist.Integral()
        f.write(str(count) + ',\n')
        print(i)
        print(count)
        print('---------------------')

    f.close()

    results.append(df.Histo1D(("recojet_isG", "", *bins_score), "recojet_isG"))

    return results, weightsum
