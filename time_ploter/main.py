import matplotlib.pyplot as plt
import numpy as np
import re

N = []
T = []
Time = []
n_last = -1
t_last = -1

indexes_t = []
indexes_n = []

time_sum = 0
first_line = True
amount = 0

with open('../cmake-build-release/jacobipar/par.txt') as f:
    for i, line in enumerate(f):
        data = re.search("N\s(\d+)\sT\s(\d+)\sTime\s+(\d\.\d+)", line)
        print("<",line,">")
        n, t, time = data.groups()
        t = int(t)
        time = float(time)
        if first_line:
            time_sum = time_sum + time
            amount = amount + 1
            first_line = False
        else:
            if t == t_last:
                time_sum = time_sum + time
                amount = amount + 1
                n = int(n)
            else:
                Time.append((n_last, t_last, time_sum/amount))
                time_sum = time
                amount = 1

        n_last, t_last = n, t
    Time.append((n_last, t_last, time_sum / amount))

Time = np.asarray(Time)
print(Time)

print("INACABADO!")

# num_bars = len(N)
#
# fig, ax = plt.subplots()
#
# ind = np.arange(num_bars)    # the x locations for the groups
# width = 0.2         # the width of the bars
# p1 = ax.bar(ind, N, width, bottom=0*cm, yerr=menStd)
#
#
# womenMeans = (145*cm, 149*cm, 172*cm, 165*cm, 200*cm)
# womenStd = (30*cm, 25*cm, 20*cm, 31*cm, 22*cm)
# p2 = ax.bar(ind + width, womenMeans, width, bottom=0*cm, yerr=womenStd)
#
# ax.set_title('Scores by group and gender')
# ax.set_xticks(ind + width / 2)
# ax.set_xticklabels(('G1', 'G2', 'G3'))
#
# ax.legend((p1[0], p2[0]), ('Men', 'Women'))
# ax.yaxis.set_units(inch)
# ax.autoscale_view()
#
# plt.show()