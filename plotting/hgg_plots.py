import ROOT

# global parameters
intLumi = 1
intLumiLabel = "L = 10 ab^{-1}"
ana_tex = "e^{+}e^{-} #rightarrow H  #rightarrow gg"
delphesVersion = "3.4.2"
energy = 125.0
collider = "FCC-ee"
formats = ["png"]

outdir = './outputs/plots/Hgg/no_cut/'
inputDir = "./outputs/histmaker/Hgg/no_cut"

plotStatUnc = True

colors = {"H": ROOT.kRed, "qq": ROOT.kGreen + 2}

procs = {"signal": {"H": ["wzp6_ee_Hgg_ecm125"]}, "backgrounds": {"qq": ["wzp6_mee_qq_ecm125"]}}

legend = {"H": "H", "qq": "qq"}

hists = {}

hists["jj_m"] = {
    "output": "jj_m",
    "logy": False,
    "stack": True,
    "rebin": 2,
    "xmin": 50,
    "xmax": 150,
    "ymin": 0,
    "ymax": 1e3,
    "xtitle": "m_{jj} (GeV)",
    "ytitle": "Events / 2 GeV",
}

hists["jj_e"] = {
    "output": "jj_e",
    "logy": False,
    "stack": True,
    "rebin": 2,
    "xmin": 0,
    "xmax": 60,
    "ymin": 0,
    "ymax": 1e9,
    "xtitle": "Energy (GeV)",
    "ytitle": "Events",
}

hists["scoresum_B"] = {
    "output": "scoresum_B",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 2.0,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{1}(B) + p_{2}(B)",
    "ytitle": "Events",
}

hists["N_G"] = {
    "output": "N_G",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 2 * 10000000,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{1}(G) + p_{2}(G)",
    "ytitle": "Events",
}

hists['Jet_One'] = {
    "output": "Jet_One",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 1.0,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{1}(G)",
    "ytitle": "Events",
}

hists['Jet_Two'] = {
    "output": "Jet_Two",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 1.0,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{2}(G)",
    "ytitle": "Events",
}

hists["jj_p"] = {
    "output":   "jj_p",
    "logy":     False,
    "stack":    True,
    "rebin":    2,
    "xmin":     0,
    "xmax":     20,
    "ymin":     0,
    "ymax":     8e3,
    "xtitle":   "p(jet) (GeV)",
    "ytitle":   "Events ",
}

hists["jj_phi"] = {
     "output": "jj_phi",
     "logy": False,
     "stack": True,
     "rebin": 2,
     "xmin": -1.5,
     "xmax": 1.5,
     "ymin": 0,
     "ymax": 100,
     "xtitle": "#phi",
     "ytitle": "Events",
 }

hists["jj_theta"] = {
     "output": "jj_theta",
     "logy": False,
     "stack": True,
     "rebin": 2,
     "xmin": 0,
     "xmax": 4,
     "ymin": 0,
     "ymax": 700,
     "xtitle": "#theta",
     "ytitle": "Events",
}

hists["jj_q"] = {
     "output": "jj_q",
     "logy": False,
     "stack": True,
     "rebin": 2,
     "xmin": -4.5,
     "xmax": 4.5,
     "ymin": 0,
     "ymax": 30e3,
     "xtitle": "charge",
     "ytitle": "Events",
}

hists["scoresum_Q"] = {
    "output": "scoresum_Q",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 2.0,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{1}(Q) + p_{2}(Q)",
    "ytitle": "Events",
}

hists["scoresum_S"] = {
    "output": "scoresum_S",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 2.0,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{1}(S) + p_{2}(S)",
    "ytitle": "Events",
}

hists["scoresum_C"] = {
    "output": "scoresum_C",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 2.0,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{1}(C) + p_{2}(C)",
    "ytitle": "Events",
}

hists["scoresum_G"] = {
    "output": "scoresum_G",
    "logy": True,
    "stack": True,
    "rebin": 1,
    "xmin": 0,
    "xmax": 2.0,
    "ymin": 1,
    "ymax": 1e9,
    "xtitle": "p_{1}(G) + p_{2}(G)",
    "ytitle": "Events",
}
