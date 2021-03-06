import matplotlib.pyplot as plt 
import sys

args = sys.argv[1]
filename, makespan, tail = args.split('.')
makespan, machines = int(makespan), []

for values in tail.split(','):
    machines.append([int(v) for v in values.split(' ')])

colors = ['#000000','#EB5D49','#97B6E7','#C99700','#0F4700','#78A35B','#ACD188','#E0FFB5','#CE050F','#D82323','#F47A5C','#D7AE27','#E4C44E','#F2DA75','#000000','#BDE6FF','#25239E','#7185CF','#FFF09B','#E14036']

nM = max([v[-1] for v in machines])
def printOperation(op_info):
    op, start, cost, job, mach = op_info
    gnt.broken_barh([(start, cost)], (mach - 0.5, 0.9), facecolors =(colors[job])) 

fig, gnt = plt.subplots() 
gnt.set_ylim(0, nM + 1) 
gnt.set_xlim(0, makespan) 
gnt.set_xlabel('time') 
gnt.set_ylabel('machines') 

gnt.set_yticks(list(range(1,nM + 1))) 
labels = [f"machine {i}" for i in range(1,nM+1)]
gnt.set_yticklabels(labels) 

gnt.grid(True) 

for mach in machines:
    printOperation(mach)

plt.savefig(f"graphs/{filename}g.jpg") 

