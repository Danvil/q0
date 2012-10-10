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
			q.append([score, count, fail, False, False])
	vals.append(q)
print vals

for i in range(len(objectives)):
	best_score = 100000000
	best_score_j = -1
	best_count = 100000000
	best_count_j = -1
	for j in range(len(algos)):
		score = vals[j][i][0]
		if score < best_score:
			best_score_j = j
			best_score = score
		count = vals[j][i][1]
		if count < best_count:
			best_count_j = j
			best_count = count
	vals[best_score_j][i][3] = True
	vals[best_count_j][i][4] = True

mode = 2

assert len(algos) == len(vals)
expr = ""
expr += "\\begin{tabular}{l"
for i in range(len(objectives)):
	expr += "l"
expr += "}\n"
expr += "\\toprule\n"
for i in range(len(objectives)):
	expr += " & "
	expr += objectives[i]
expr += "\\\\\n"
expr += "\\midrule\n"
for i in range(len(algos)):
	expr += algos[i] 
	for x in vals[i]:
		expr += " & "
		if x[3+mode-1]:
			expr += "{\\bf "
		if mode == 1:
			expr += ("%.3f" % x[0])
		else:
			expr += ("%.0f" % x[1])
			if x[2] > 0:
				expr += " (" + str(x[2]) + ")"
		if x[3+mode-1]:
			expr += "}"
	expr += "\\\\\n"
expr += "\\bottomrule\n"
expr += "\end{tabular}\n"

print expr
