#!/usr/bin/env python
import sys
import csv
import matplotlib
matplotlib.use('Agg') # don't try to display it
import matplotlib.pyplot as plt
import re

def parse_file(f):
	t_0 = None # the first timestamp in the CSV, all other values are printed as deltas to this

	data = {}
	for line in f:
		# various other output, e.g. the robot id after it was created
		if len(line) < 4:
			continue
		# get the first timestamp
		if t_0 is None:
			t_0 = int(line[0])

		t = (int(line[0]) - t_0) 
		#fuel = int(line[1])
		distance = float(line[2])
		#currentPosition = line[3]

		# there's multiple measurements per second, however the time
		# is given in whole seconds. So just assume that it's as
		# an 100ms intervall
		# I go through the file line by line, so I don't which one of
		# the measurements for this second is there right now. So just
		# try to input them one after the other
		for i in xrange(0, 10):
			subsecondresulotion = (float(t) + i / 10)
			if subsecondresulotion in data:
				continue

			data[subsecondresulotion] = distance
	
	return data

def min_max_from_2d_dict(d):
	# get those keys that exist in all dicts
	keys = set()
	for k, v in d.iteritems():
		if len(keys) == 0:
			keys |= set(v.keys())
		else:
			keys &= set(v.keys())
	
	# find min, max for each key
	mins = []
        maxs = []
	for key in keys:
		vals = sorted([d[i][key] for i in xrange(0, len(d))])
		mins.insert(int(key), vals[0])
		maxs.insert(int(key), vals[-1])
		#ret.insert(int(key), vals[len(vals) / 2])
		#ret.insert(int(key), sum(vals) / len(vals))

	return list(keys), mins, maxs


if __name__ == '__main__':
	if len(sys.argv) < 2:
		print "%s samples in ..." % (sys.argv[0])
		sys.exit(0)

        samples = int(sys.argv[1])

	lines = {}
	pattern = re.compile("\d+") # filename is "fi_%p%_%n%.csv"
	for filename in sys.argv[2:]:
		p, n = [int(x) for x in pattern.findall(filename)]
		print p, n

		if p not in lines:
			lines[p] = {}

		f = csv.reader(open(filename), delimiter=';')
		lines[p][n] = parse_file(f)

	colors = ['b', 'r', 'm', 'k', 'c', 'g']
	for p, measurements in lines.iteritems():
		color = colors[p / 10] # p is in steps of ten, so divide to get the index
		x, y_min, y_max = min_max_from_2d_dict(measurements)
                plt.fill_between(x[:samples], y_min[:samples], y_max[:samples], where=y_max>y_min, facecolor=color, label="%i%%" % (p), alpha=0.3)

                plt.plot(x[:samples:3], y_min[:samples:3], color + '.-')
                plt.plot(x[:samples:3], y_max[:samples:3], color + '.-')

	leg = plt.legend(loc='best', shadow=True, fancybox=True)
	leg.get_frame().set_alpha(0.5)

        plt.grid(color='gray')
	plt.xlabel("Zeit (in s)")
	plt.ylabel("Zurueckgelegte Entfernung (in m)")
	plt.suptitle("Zurueckgelegte Entfernung mit steigender Fehlerwahrscheinlichkeit")

	plt.savefig("zemsf.png")
