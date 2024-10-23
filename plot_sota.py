#!/usr/bin/env python

import math
import os
import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import operator
from operator import truediv
from operator import add
from operator import itemgetter
from itertools import islice
import collections
import pandas as pd
from matplotlib.lines import Line2D
from mpl_toolkits.axes_grid1.inset_locator import zoomed_inset_axes
from mpl_toolkits.axes_grid1.inset_locator import mark_inset
from pandas.plotting import table
import matplotlib.patches as mpatches
from matplotlib.patches import Patch
import matplotlib.ticker as ticker
import matplotlib.lines as mlines
import seaborn as sns
import matplotlib.markers as mmark
import matplotlib.lines as mlines

ot = pd.read_csv('speedup_sota.csv')

plt.rcParams.update({'font.size':18})
fig, ax = plt.subplots(2)
ax = plt.figure(figsize=(7.5, 2.8)).add_subplot(111)
ax.clear()
plt.tight_layout()

final = pd.DataFrame(ot)

color_list = 10*['k']
marker_list = ["-*", '-|', '-_', '-.', '-X', '-<']
final.plot(kind='line', ax=ax, style=marker_list, markersize=6, legend=False, color=color_list, linewidth=0.2)

plt.ylim([-2,10])
plt.yticks(np.arange(-2.01, 10.01, 2))

circ1 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker="*", linestyle=' ', label='SP', markersize=12)
circ2 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='|', linestyle=' ', label='ASP', markersize=12)
circ3 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='_', linestyle=' ', label='DP', markersize=12)
circ4 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='.', linestyle=' ', label='MP', markersize=12)
circ5 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='X', linestyle=' ', label='Morrigan', markersize=11)

legend_elements = [circ1, circ2, circ3, circ4,circ5]

plt.legend(handles=legend_elements, loc='center', bbox_to_anchor=(0.5, 1.08), ncol=6, fontsize=12)

ax.set_axisbelow(True)
ax.grid(which='major', axis='y', linestyle='-', drawstyle="steps", linewidth=0.0000004, color='w')
ax.set_ylabel('% speedup')

ax.set_xlabel('QMM server workloads')
plt.xticks(np.arange(-0.1, 45))
plt.xticks(np.arange(0, 45))
plt.xlim([-0.3,44.5])
ax.set_xticklabels([], rotation=0, fontsize=12)

plt.savefig('speedup_sota'+".pdf", bbox_inches="tight", format='pdf', dpi=1000)
