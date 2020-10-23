import pandas as pd
import numpy as np

data = pd.read_csv("results/random.csv")
rows = data.to_numpy()

medians = []

for row in rows:

