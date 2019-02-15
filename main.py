"""
    Loads a frags file to a dataframe, creates the diagonal column,
    sorts it regarding diagonal and xStart column, divides it in n
    files and runs them. Finally, the files are concatenated and the
    suitable
"""

from frags_to_csb import frags_to_csb
import pandas as pd
import numpy as np
import argparse

parser = argparse.ArgumentParser(description='asd')
parser.add_argument('header_filename', type=str, nargs=1, help='Absolute or relative path to the file containing the gecko frag file header')
parser.add_argument('frags_filename', type=str, nargs=1, help='Absolute or relative path to the file containing the gecko frag file frags (csv)')

args = parser.parse_args()
header_filename = args.header_filename[0]
frags_filename = args.frags_filename[0]

x_len = 0
y_len = 0

with open(header_filename, 'r') as header_file:
    for line in header_file:
        if 'SeqX length' in line:
            x_len = int(line.split()[3])
        if 'SeqY length' in line:
            y_len = int(line.split()[3])

df = pd.read_csv(frags_filename)

df = df.loc[df['strand(f/r)'] == 'f']
df['diagonal'] = df['xStart'] - df['yStart'] // (np.sqrt(max(x_len, y_len)))

df_sort_diag_x = df.sort_values(['diagonal', 'xStart'])

df_sort_diag_x.to_csv('sorted_frags.csv', index = False)

df_csb = frags_to_csb(df_sort_diag_x, x_len, y_len)

df_csb.to_csv('temp_csb.csv', index = False)
filenames = [header_filename, 'temp_csb.csv']
with open('csb_final.csv', 'w') as outfile:
    for fname in filenames:
        with open(fname) as infile:
            for line in infile:
                outfile.write(line)