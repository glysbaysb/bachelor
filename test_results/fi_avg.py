#!/usr/bin/env python
import sys
import csv
import matplotlib
matplotlib.use('Agg') # don't try to display it
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from matplotlib.patches import Patch
import numpy as np
import re
import random

def parse_file(f):
	sum_ = 0
        cnt = 0
	for line in f:
		# various other output, e.g. the robot id after it was created
		if len(line) < 3:
			continue

                cnt = cnt + 1
                sum_ = sum_ + long(line[3])
	
	return sum_ / cnt

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print "%s in ..." % (sys.argv[0])
		sys.exit(0)

        ''' parse files '''
	data = {}
        data["Erwartet"] = 1000
	for filename in sys.argv[1:]:
		label = raw_input("Name fuer %s:" % (filename,))

		f = csv.reader(open(filename), delimiter=';')
		data[label] = (parse_file(f) / 1e6)

        ''' Add the measurements '''
	colors = [('Greys', 'grey'),
                ('Blues', 'blue'),
                ('Greens', 'green'),
                ('Reds', 'red'),
                ('Oranges', 'orange'),
				('Purples', 'purple'),
        ]

        plt.rcdefaults()
	fig, ax = plt.subplots()

        y_pos = np.arange(len(data))
        ax.set_yticks(y_pos)

        ''' draw bars '''
        ax.barh(y_pos, data.values(), align='center')
        for i, v in enumerate(data.values()):
            ax.text(500, i + 1/len(data), str(round(v, 2)), color='black', fontweight='bold')

        ''' Labels '''
        ax.set_yticklabels(data.keys())
	supt = raw_input("Titel: ")
	plt.suptitle(supt)

        ''' write plot '''
	out = raw_input("Dateiname:")
	plt.savefig(out + ".png")
