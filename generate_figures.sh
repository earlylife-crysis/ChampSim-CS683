python3 speedup.py Statistics/ no64nofp sp64nofp asp64nofp dp64nofp markov_sota64nofp morriganPT64fp; python3 plot_sota.py ; rm speedup_sota.csv

python3 speedup.py Statistics/ no64nofp no64nofpISO morriganPT64fpP2TLB no64nofpASAP morriganPT64fp morriganPT64fpASAP no64nofpIDEAL; python3 plot_other_techniques.py; rm other_techniques.csv

mkdir -p Figures
mv *.pdf ./Figures
