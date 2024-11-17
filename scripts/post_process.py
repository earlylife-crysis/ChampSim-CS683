# Python script to extract simulation results

import csv
import os

logs_path = "../logs/agile_eval/spec17"

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

    append_list.append(trace.split(".")[1])

    # Iterate through each log
    for log in sorted(os.listdir()):
        logfile = open(log, 'r')

        if counter == 0:
            if log == "asp-NaiveFP.txt":
                csv_header.append("ASP NaiveFP")
            if log == "asp-NoFP.txt":
                csv_header.append("ASP NoFP")
            if log == "asp-SBFP.txt":
                csv_header.append("ASP SBFP")
            if log == "agile-NaiveFP.txt":
                csv_header.append("Agile NaiveFP")
            if log == "agile-NoFP.txt":
                csv_header.append("Agile NoFP")
            if log == "agile-SBFP.txt":
                csv_header.append("Agile SBFP")
            if log == "dp-NaiveFP.txt":
                csv_header.append("DP NaiveFP")
            if log == "dp-NoFP.txt":
                csv_header.append("DP NoFP")
            if log == "dp-SBFP.txt":
                csv_header.append("DP SBFP")
            if log == "no-NaiveFP.txt":
                csv_header.append("No NaiveFP")
            if log == "no-NoFP.txt":
                csv_header.append("No NoFP")
            if log == "no-SBFP.txt":
                csv_header.append("No SBFP")
            if log == "sp-NaiveFP.txt":
                csv_header.append("SP NaiveFP")
            if log == "sp-NoFP.txt":
                csv_header.append("SP NoFP")
            if log == "sp-SBFP.txt":
                csv_header.append("SP SBFP")

            # # Conditional Statements for Morrigan eval
            # if log == "asp_istlb.txt":
            #     csv_header.append("ASP")
            # if log == "dp_istlb.txt":
            #     csv_header.append("DP")
            # if log == "no.txt":
            #     csv_header.append("No")
            # if log == "sp_istlb.txt":
            #     csv_header.append("SP")
            # if log == "markov_sota_istlb.txt":
            #     csv_header.append("MP")
            # if log == "morriganPT.txt":
            #     csv_header.append("Morrigan")

        for line in logfile:
            # if "STLB LOAD      ACCESS:" in line:
            #     append_list.append(line.split()[7])
            if "CPU 0 cumulative IPC:" in line:
                append_list.append(line.split()[4])

    os.chdir("..")
    counter = counter + 1
    csv_data.append(append_list)

# Write stats to a CSV file in the same directory
with open("stats.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(csv_header)
    writer.writerows(csv_data)