import csv

data = []
with open('/home/david/Projects/q0/release/examples/eval.tsv','r') as f:
	spamreader = csv.reader(f, delimiter='\t')
	for row in spamreader:
		data.append(row)
data = data[1:]

for i in range(len(data)):
	data[i][0] = data[i][0].replace('_', ' ')
	data[i][1] = data[i][1].replace('_', ' ')

print data

objectives = []
for x in data:
	if x[0] not in objectives:
		objectives.append(x[0])
print 'Objectives:', ', '.join(objectives)

algos = []
for x in data:
	if x[1] not in algos:
		algos.append(x[1])
print 'Algos:', ', '.join(algos)

# for each algo a list of pairs (score,count) - one pair per function
vals = []
for a in algos:
	q = []
	for f in objectives:
		found = False
		for x in data:
			if x[0] == f and x[1] == a:
				score = float(x[2])
				count = float(x[3])
				fail = int(x[4])
				found = True
			if found: break
		if not found:
			print "ARGH"
		else:
			s = ("%.0f" % count)
			if fail > 0:
				s += " (" + str(fail) + ")"
			q.append([("%.3f" % score), s])
	vals.append(q)
print vals

mode = 2

assert len(algos) == len(vals)
str = ""
str += "\\begin{tabular}{l"
for i in range(len(objectives)):
	str += "l"
str += "}\n"
str += "\\toprule\n"
for i in range(len(objectives)):
	str += " & "
	str += objectives[i]
str += "\\\\\n"
str += "\\midrule\n"
for i in range(len(algos)):
	str += algos[i] 
	for x in vals[i]:
		if mode == 1:
			str += " & " + x[0]
		else:
			str += " & " + x[1]
	str += "\\\\\n"
str += "\\bottomrule\n"
str += "\end{tabular}\n"

print str
