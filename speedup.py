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
import csv
import matplotlib.markers as mmark
import matplotlib.lines as mlines


qmm_to_use = ['srv_575', 'srv_743', 'srv_551', 'srv_526', 'srv_73', 'srv_706', 'srv_128', 'srv_194', 'srv_287', 'srv_764', 'srv_617', 'srv_276', 'srv_364', 'srv_408', 'srv_669', 'srv_426', 'srv_715', 'srv_504', 'srv_771', 'srv_48', 'srv_222', 'srv_255', 'srv_225', 'srv_41', 'srv_32', 'srv_259', 'srv_641', 'srv_582', 'srv_21', 'srv_702', 'srv_61', 'srv_442', 'srv_495', 'srv_727', 'srv_537', 'srv_540', 'srv_85', 'srv_12', 'srv_207', 'srv_s7', 'srv_s69', 'srv_s0', 'srv_s10', 'srv_s61', 'srv_s60']

def take(n, iterable):
    "Return first n items of the iterable as a list"
    return list(islice(iterable, n))

def geo_mean(iterable):
    a = np.array(iterable)
    return a.prod()**(1.0/len(a))


def raw_data(benchmark_suite, baseline, prefetchers, root_dir):
    #get the raw data of multiple prefetchers
    P_res = []

    baseline_dir = root_dir + '/' + baseline

    for filename in os.listdir(baseline_dir):

        path = os.path.join(baseline_dir, filename)

        if os.path.isdir(path) or filename.endswith(".py"):
            continue

        filename_list = filename.split('.')

        if ((filename_list[0] in qmm_to_use) == False) and benchmark_suite == 'qmm':
            continue

        # first element the baseline case and then all the prefetchers
        cycles_per_pref = []
        speed_up = []
        for t in range(len(prefetchers)):
            cycles_per_pref.append(0)
            speed_up.append(0.0)

        path_to_search = baseline_dir
        os.chdir(path_to_search)

        fp = open(filename)
        line = fp.readline()
        while line:
            if line.strip():
                tokens = line.split()
                if tokens[0] == "CPU" and tokens[1] == "0" and tokens[2] == "cumulative" and tokens[7] == "cycles:":
                    baseline = float(tokens[8])
            line = fp.readline()

        for index, val in enumerate(prefetchers):
            path_to_search = root_dir + '/' + val
            os.chdir(path_to_search)
            fp = open(filename)
            line = fp.readline()
            while line:
                if line.strip():
                    tokens = line.split()
                    if tokens[0] == "CPU" and tokens[1] == "0" and tokens[2] == "cumulative" and tokens[7] == "cycles:":
                        cycles_per_pref[index] = float(tokens[8])
                line = fp.readline()

        speed_up = [baseline / x for x in cycles_per_pref]
        speed_up.insert(0, filename.split('.')[0])


        P_res.append(speed_up)
        os.chdir(root_dir)

    P_res = sorted(P_res, key=itemgetter(0))
    return P_res



# This is the main
root_path = os.getcwd()

baseline = sys.argv[2]
prefetcher_list = sys.argv[3:]

benchmark_suite = 'qmm'
folder_path = root_path + '/' + '/' + sys.argv[1]
w = raw_data(benchmark_suite, baseline, prefetcher_list, folder_path)

list_of_dictionaries_qmm = []

for index, val in enumerate(prefetcher_list):
    list_of_dictionaries_qmm.append({})

for r in range(len(w)):
    if w[r][0].split('_')[0][0] == 's':
        name = w[r][0].split('_')[0] + '_' + w[r][0].split('_')[1]# name of the ipc_increment_benchmark
    else:
        name = w[r][0].split('_')[0] + '_' + w[r][0].split('_')[1] + '_' + w[r][0].split('_')[2] # name of the ipc_increment_benchmark
    if r > 0:
        temp_m = w[r-1][0].split('_')[0] + '_' + w[r-1][0].split('_')[1] # name of the ipc_increment_benchmark
        if name != temp_m:
            for kk in range(len(prefetcher_list)):
                list_of_dictionaries_qmm[kk][name] = 0
                list_of_dictionaries_qmm[kk][name] = list_of_dictionaries_qmm[kk][name] + w[r][kk+1]

        else:
            for kk in range(len(prefetcher_list)):
                list_of_dictionaries_qmm[kk][name] = list_of_dictionaries_qmm[kk][name] + w[r][kk+1]

    else:
        for kk in range(len(prefetcher_list)):
            list_of_dictionaries_qmm[kk][name] = 0
            list_of_dictionaries_qmm[kk][name] = list_of_dictionaries_qmm[kk][name] + w[r][kk+1]


df4 = pd.DataFrame(list_of_dictionaries_qmm)
df4 = df4.T

df4.columns = prefetcher_list

geo_mean_list= []
for i in range(len(prefetcher_list)):
    temp_list = df4.ix[:,int(i)]
    geo_mean_list.append(geo_mean(temp_list))
df4.loc['GeoMean'] = geo_mean_list

df4 = df4.sub(1)
df4 = df4.rmul(100)

print(df4)

df4 = df4.loc[qmm_to_use]

os.chdir(root_path)
if sys.argv[-1][-1] == 'L':
    df4.to_csv('other_techniques.csv', sep=',', mode='a', index=False)
else:
    df4.to_csv('speedup_sota.csv', sep=',', mode='a', index=False)
