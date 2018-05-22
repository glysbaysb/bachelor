#!/usr/bin/env python
import sys
import csv
import matplotlib
matplotlib.use('Agg') # don't try to display it
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import re
import random

def parse_file(f):
	data = []
	for line in f:
		# various other output, e.g. the robot id after it was created
		if len(line) < 3:
			continue

		currentPosition = line[2].replace('(', '').replace(')', '').split(':')
		data.append([float(currentPosition[0]), float(currentPosition[1])]) 
	
	return data

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print "%s in ..." % (sys.argv[0])
		sys.exit(0)

	data = {}
	for filename in sys.argv[1:]:
		label = raw_input("Name fuer %s:" % (filename,))

		f = csv.reader(open(filename), delimiter=';')
		data[label] = parse_file(f)

	colors = ['b', 'r', 'g', 'k']
	fig, ax = plt.subplots()

	for name, measurement in data.iteritems():
		print name, len(measurement)
		x, y = zip(*measurement)
		ax.scatter(x, y, label=name, color=colors.pop())
	
	ax.margins(0.05)
	ax.grid(color='grey')

	ax.xaxis.set_major_formatter(ticker.FormatStrFormatter('%0.1f'))
	ax.xaxis.set_major_locator(ticker.MultipleLocator(base=2.5))
	ax.yaxis.set_major_formatter(ticker.FormatStrFormatter('%0.1f'))
	ax.yaxis.set_major_locator(ticker.MultipleLocator(base=2.5))

	leg = plt.legend(loc='best', shadow=True, fancybox=True)
	leg.get_frame().set_alpha(0.5)

	plt.xlabel("X-Position")
	plt.ylabel("Y-Position")
	supt = raw_input("Titel: ")
	plt.suptitle(supt)

	out = raw_input("Dateiname:")
	plt.savefig(out + ".png")
