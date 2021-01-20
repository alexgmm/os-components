import dataframe_image as dfi
import pandas as pd
import numpy as np

#pd.set_option("display.precision", 2)

class Instance:
    def __init__(self, row, columns):
        def getResultsSet(results):
            sa, ts, ils = results[:20], results[20:40], results[40:]
            self.resultsSet = { 
                                'SA': { 'swap':sa[:5], 'n5':sa[5:10], 'n7':sa[10:15], 'shift':sa[15:20] },
                                'TS': { 'swap':ts[:5], 'n5':ts[5:10], 'n7':ts[10:15], 'shift':ts[15:20] },
                                'ILS': { 'swap':ils[:5], 'n5':ils[5:10], 'n7':ils[10:15], 'shift':ils[15:20] } 
                              }

            self.stats = { 
                            'SA':  { 'swap': None, 'n5': None, 'n7': None, 'shift': None },
                            'TS':  { 'swap': None, 'n5': None, 'n7': None, 'shift': None },
                            'ILS': { 'swap': None, 'n5': None, 'n7': None, 'shift': None } 
                         }

            for h in self.resultsSet.keys():
                for o in self.resultsSet[h].keys():
                    r = self.resultsSet[h][o]
                    mean, median = np.mean(r), np.median(r)
                    percentDeviation = (mean / self.best)*100 - 100
                    self.stats[h][o] = {'mean': mean, 'median': median, f"%dev": percentDeviation}

        def getSize():
            s = self.filename
            if "tai" in s:
                self.size = int(s[3:5])
            elif "per" in s:
                self.size = int(s[1])
            else:
                self.size = int(s[2:4])
        self.filename, self.best, *results = row
        getSize()
        getResultsSet(results)

    def getResultsRow(self,stat):
        row = []
        for h in self.stats.keys():
            for nb in self.stats[h].keys():
                row.append(self.stats[h][nb][stat])

        return row

    def __getitem__(self, arg):
        heuristic, nb, stat = arg
        return self.stats[heuristic][nb][stat]

def tuple2str(t):
    a, b = t
    return f"{a}:{b}"

def highlight_min(data):
    color= 'red'
    attr = 'background-color: {}'.format(color)

    if data.ndim == 1:  # Series from .apply(axis=0) or axis=1
        is_min = data == data.min()
        return [attr if v else '' for v in is_min]
    else: 
        is_min = data.groupby(level=0).transform('min') == data
        return pd.DataFrame(np.where(is_min, attr, ''),
                            index=data.index, columns=data.columns)


class DataFrame:
    def __init__(self, src = "results/all.csv"):
        def getInstances():
            data = pd.read_csv("results/all.csv")
            instances = []

            for index, row in enumerate(data.to_numpy()):
                """ if index == 10:
                    break """
                instances.append(Instance(row=list(row), columns = list(data.columns)))
            
            return instances

        self.instances = getInstances()
        self.heuristics, self.nbs = ['SA', 'TS', 'ILS'], ['swap', 'n5', 'n7', 'shift']
    
    def savePercentDevTable(self):
        def f(o):
            print(o)
            return o

        stats = [f"%dev"]
        keys = []

        instanceNames = []
        instanceSizes = []

        for h in self.heuristics:
            for nb in self.nbs:
                for s in stats:
                    keys.append((h, nb))

        topHeader = ['','',*self.nbs,*self.nbs,*self.nbs]#['','',*['SA']*4,*['TS']*4,*['ILS']*4]
        bottomHeader = ['file','size', *self.nbs*3]

        cols = pd.MultiIndex.from_arrays([ topHeader, bottomHeader ])

        rows = []

        for i in self.instances:
            rows.append([i.filename, i.size] + i.getResultsRow(f"%dev"))
    
        df = pd.DataFrame(rows, columns=cols)
        df_csv = df.to_csv(path_or_buf="percent_dev.csv",sep="#",float_format="%.2f")
        #df = df.reset_index(drop=True).style.apply(highlight_min, axis=0)
        #dfi.export(df, 'percentDev.png', max_rows=-1)

d = DataFrame()
d.savePercentDevTable()