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

ot = pd.read_csv('other_techniques.csv')

plt.rcParams.update({'font.size':18})
fig, ax = plt.subplots(2)
ax = plt.figure(figsize=(7.5, 3.5)).add_subplot(111)
ax.clear()
plt.tight_layout()

final = pd.DataFrame(ot)

color_list = ['#f7f7f7', '#a7a7a7', '#686868', '#484848', '#212121', '#000000']
color_list = ['darkblue','maroon','seagreen', 'peru', 'beige']
color_list = ['whitesmoke', 'silver', 'darkgray', 'gray', 'dimgray']
color_list = ['k', 'b', 'y', 'r', 'g', 'magenta', '#dedede', '#a7a7a7', '#686868', '#f7f7f7', '#dedede', '#a7a7a7', '#686868', 'black']
color_list = 10*['k']
marker_list = ["-*", '-|', '-X', '-o', '-<']
marker_list = ["-*", '-|', '-_', '-X', '-o', '-<']
final.plot(kind='line', ax=ax, style=marker_list, markersize=6, legend=False, color=color_list, linewidth=0.2)

plt.ylim([-25,15])
plt.yticks(np.arange(-25, 15.01, 5))

circ1 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='*', linestyle=' ', label='ISO-Storage', markersize=9)
circ11 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='|', linestyle=' ', label='P2TLB', markersize=9)
circ2 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='_', linestyle=' ', label='ASAP', markersize=9)
circ3 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='X', linestyle=' ', label='Morrigan', markersize=9)
circ4 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='.', linestyle=' ', label='Morrigan+ASAP', markersize=10)
circ5 = mlines.Line2D([], [], color='black', markeredgecolor='black', marker='<', linestyle=' ', label='Perfect L2-TLB-I', markersize=8 )

legend_elements = [circ1, circ11, circ2, circ3, circ4,circ5]

plt.legend(handles=legend_elements, loc='center', bbox_to_anchor=(0.5, 1.12), ncol=3, fontsize=13)

ax.set_axisbelow(True)
ax.grid(which='major', axis='y', linestyle='-', drawstyle="steps", linewidth=0.0000004, color='w')
ax.set_ylabel('% speedup')

ax.set_xlabel('QMM server workloads')
plt.xticks(np.arange(-0.3, 45))

ax.set_xticklabels([], rotation=0, fontsize=12)
plt.xticks(np.arange(0, 45))
plt.xlim([-0.3,44.5])
plt.savefig('plot_other_techniques'+".pdf", bbox_inches="tight", format='pdf', dpi=1000)
