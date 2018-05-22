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
		if len(data) > 200:
			break

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

def mean_from_2d_dict(d):
	# get those keys that exist in all dicts
	keys = set()
	for k, v in d.iteritems():
		if len(keys) == 0:
			keys |= set(v.keys())
		else:
			keys &= set(v.keys())
	
	# find mean for each key
	ret = []
	for key in keys:
		vals = [d[i][key] for i in xrange(0, len(d))]
		#ret.insert(int(key), vals[len(vals) / 2])
		ret.insert(int(key), sum(vals) / len(vals))

	return list(keys), ret


if __name__ == '__main__':
	if len(sys.argv) < 2:
		print "%s in ..." % (sys.argv[0])
		sys.exit(0)

	lines = {}

	pattern = re.compile("\d+") # filename is "fi_%p%_%n%.csv"
	for filename in sys.argv[1:]:
		p, n = [int(x) for x in pattern.findall(filename)]
		print p, n

		if p not in lines:
			lines[p] = {}

		f = csv.reader(open(filename), delimiter=';')
		lines[p][n] = parse_file(f)

	colors = ['b', 'r', 'm', 'k', 'c', 'g']
	styles = ['.', 'o', 'v', '*', 'd', 'H']
	for p, measurements in lines.iteritems():
		color = colors[p / 10] # p is in steps of ten, so divide to get the index
		'''
		plt.plot(measurements[0].keys(), measurements[0].values(), color, label="%i%%" % (p))
		'''
		'''
		for n, measurement in measurements.iteritems():
			style = styles[n]
			if n == 0:
				plt.plot(measurement.keys(), measurement.values(), color + style, label="%i%%" % (p))
			else:
				plt.plot(measurement.keys(), measurement.values(), color + style)
		'''
		style = styles[p / 10]
		x, y = mean_from_2d_dict(measurements)
		plt.plot(x[:60], y[:60], color, label="%i%%" % (p))

	leg = plt.legend(loc='best', shadow=True, fancybox=True)
	leg.get_frame().set_alpha(0.5)

	plt.xlabel("Zeit (in s)")
	plt.ylabel("Zurueckgelegte Entfernung (in m)")
	plt.suptitle("Zurueckgelegte Entfernung mit steigender Fehlerwahrscheinlichkeit")

	plt.savefig("zemsf.png")
