# Python script to extract simulation results

import csv
import os

logs_path = "../logs/QMM"

os.chdir(logs_path)

csv_header = []
csv_data = []

csv_header.append("Trace")
counter = 0

# Iterate through each folder
for trace in os.listdir():
    os.chdir(trace)

    # Create list to hold stats
    append_list = []

    append_list.append(trace.split(".")[0])

    # Iterate through each log
    for log in sorted(os.listdir()):
        logfile = open(log, 'r')

        if counter == 0:
            if log == "asp_sbfp-0threshold.txt":
                csv_header.append("ASP AllFP")
            if log == "asp_sbfp-disabled.txt":
                csv_header.append("ASP NoFP")
            if log == "asp_sbfp-enabled.txt":
                csv_header.append("ASP SBFP")
            if log == "dp_sbfp-0threshold.txt":
                csv_header.append("DP AllFP")
            if log == "dp_sbfp-disabled.txt":
                csv_header.append("DP NoFP")
            if log == "dp_sbfp-enabled.txt":
                csv_header.append("DP SBFP")
            if log == "no_sbfp-0threshold.txt":
                csv_header.append("No AllFP")
            if log == "no_sbfp-disabled.txt":
                csv_header.append("No NoFP")
            if log == "no_sbfp-enabled.txt":
                csv_header.append("No SBFP")
            if log == "sp_sbfp-0threshold.txt":
                csv_header.append("SP AllFP")
            if log == "sp_sbfp-disabled.txt":
                csv_header.append("SP NoFP")
            if log == "sp_sbfp-enabled.txt":
                csv_header.append("SP SBFP")

        for line in logfile:
            if "STLB LOAD      ACCESS:" in line:
                append_list.append(line.split()[7])

    os.chdir("..")
    counter = counter + 1
    csv_data.append(append_list)

# Write stats to a CSV file in the same directory
with open("stats.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(csv_header)
    writer.writerows(csv_data)