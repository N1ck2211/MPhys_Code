import ROOT

# global parameters
intLumi = 10000000
intLumiLabel = "L = 10 ab^{-1}"
ana_tex = "e^{+}e^{-} #rightarrow H #rightarrow gg"
delphesVersion = "3.4.2"
energy = 125.0
collider = "FCC-ee"
formats = ["png"]

inputDir = "./full_clustering_output/histmaker"
outdir = './full_clustering_output/plots'

plotStatUnc = True

colors = {"H": ROOT.kRed, "qq": ROOT.kBlue}

procs = {}
procs = {"signal": {"H": ["wzp6_ee_Hgg_ecm125"]}, "backgrounds": {"qq": ["wzp6_ee_qq_ecm125"]}}
legend = {"H": "H", "qq": "Z/#gamma"}

hists = {}

hists["cutFlow"] = {
    "output":   "cutFlow",
    "logy":     True,
    "stack":    True,
    "xmin":     0,
    "xmax":     10,
    "ymin":     1e11,
    "ymax":     1e18,
    "xtitle":  ["All_events","CHad", "nconst", "jet_p", "jet_pt", "jet_e", "q_score", "g_score", "jet_mass", "jj_mass", "jet_eta"],
    "ytitle":   "Events ",
    "scaleSig": 1
}
