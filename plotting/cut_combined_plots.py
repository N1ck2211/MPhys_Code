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

#colors = {"bb": ROOT.kRed, "qq": ROOT.kGreen + 1, "gg": ROOT.kYellow + 1, "cc":  ROOT.kBlue, "ss":  ROOT.kViolet}

colors = {"H": ROOT.kRed, "qq": ROOT.kBlue}

process_list = ["wzp6_ee_qq_ecm125", "wzp6_ee_Hgg_ecm125",]

procs = {}
#procs = {"signal": {"qq": ["wzp6_ee_qq_ecm125_qq"], "gg": ["wzp6_ee_qq_ecm125_gg"], "ss": ["wzp6_ee_qq_ecm125_ss"], "cc": ["wzp6_ee_qq_ecm125_cc"], "bb": ["wzp6_ee_qq_ecm125_bb"]}, "backgrounds": {}}
#legend = {"qq": "uu/dd", "ss": "ss", "cc": "cc", "bb": "bb", "gg": "gg"}

procs = {"signal": {"H": ["wzp6_ee_Hgg_ecm1253"]}, "backgrounds": {"qq": ["wzp6_ee_qq_ecm125"]}}
legend = {"H": "H", "qq": "Z/#gamma"}

hists = {}

hists["jj_m"] = {
   "output": "jj_m_background",
   "logy": False,
   "stack": True,
   "rebin": 2,
   "xtitle": "m_{jj} (GeV)",
   "ytitle": "Events / 2 GeV",
}
