import pandas as pd
import numpy as np

names_1 = ['label']
#names_2 = ['jet_nconst_1', 'jet_nconst_2']

df_1 = pd.read_csv('combined_background.csv', index_col=False)

#For two collumns
#df_2 = pd.read_csv('jet_nconst.csv', names=names_2, index_col=False)

# For single columns:
df_3 = pd.read_csv('output.csv', names=names_1, index_col=False)


res = pd.concat([df_1, df_3], axis=1).reindex(df_1.index)
res.to_csv('combined_background.csv', index=False)
