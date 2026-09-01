#!/usr/bin/env python3
import argparse, csv, glob, statistics
from collections import defaultdict

ap=argparse.ArgumentParser(description='Summarize captured MDS2450 CSV results')
ap.add_argument('files',nargs='*',help='results/*.csv; default: results/mds2450_*.csv')
a=ap.parse_args()
files=a.files or glob.glob('results/mds2450_*.csv')
if not files:
    raise SystemExit('No CSV files found.')

vals=defaultdict(list)
for f in files:
    with open(f,newline='',encoding='utf-8') as fp:
        for row in csv.DictReader(fp):
            try:
                vals[(int(row['case']),row['benchmark'])].append(int(row['ticks']))
            except (KeyError,ValueError):
                pass

med={k:statistics.median(v) for k,v in vals.items() if v}
print('case,benchmark,n,median_ticks,improvement_vs_case1_pct')
for (case,bench),m in sorted(med.items()):
    base=med.get((1,bench))
    imp='' if base is None else f'{(base-m)/base*100:.2f}'
    print(f'{case},{bench},{len(vals[(case,bench)])},{m},{imp}')
