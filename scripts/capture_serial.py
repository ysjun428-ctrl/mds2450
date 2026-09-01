#!/usr/bin/env python3
import argparse, csv, datetime, pathlib, serial, sys

ap=argparse.ArgumentParser(description='Capture MDS2450 UART and extract CSV lines')
ap.add_argument('--dev',default='/dev/ttyUSB0')
ap.add_argument('--baud',type=int,default=115200)
ap.add_argument('--out',default='results')
a=ap.parse_args()

out=pathlib.Path(a.out); out.mkdir(parents=True,exist_ok=True)
ts=datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
log=out/f'mds2450_{ts}.log'; csvp=out/f'mds2450_{ts}.csv'
print(f'capture {a.dev}@{a.baud} -> {log}')

with serial.Serial(a.dev,a.baud,timeout=0.5) as ser, log.open('w',encoding='utf-8') as lf, csvp.open('w',newline='',encoding='utf-8') as cf:
    w=csv.writer(cf); w.writerow(['case','benchmark','run','ticks'])
    try:
        while True:
            raw=ser.readline()
            if not raw: continue
            line=raw.decode('utf-8','replace').rstrip('\r\n')
            print(line); lf.write(line+'\n'); lf.flush()
            if line.startswith('CSV,'):
                parts=line.split(',')
                if len(parts)==5: w.writerow(parts[1:]); cf.flush()
    except KeyboardInterrupt:
        print(f'\nsaved: {log}\nsaved: {csvp}')
