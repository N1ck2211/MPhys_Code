# MPhys_Code
A Backup Repo for code used during my masters' thesis.

## Treemaker files:
- Contains treemaker files, which are used to apply jet tagging and clustering algorithms, get features already present in Winter2023 dataset, and compute combined jet mass. These features are then returned in flat NTuple File format.

## Histmaker files:
- Apply cuts to features in flat NTuple files, produces and returns TH1D histmaker object for each feature. Weights of each histogram are scled by luminosity/cross section to give true number of expected events.

## Plotting Files
- Various files to apply root plotting framework

## BDT_files:
- Jupyter notebooks that apply XGBoost algorithm to dataset.

## Neural Net Code:
- Juptyer noterbooks that use Keras to build and train a multilayer perceptron model.

## Misc:
- Other general files.
- Includes various python scripts to join large panda dataframes togther, and various plotting scripts to produce plots for the final report.
