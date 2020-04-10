f = open("output.txt")
lines = f.readlines()
f.close()

marker1 = "<<"
marker2 = ">>"

typeCol = 0
addrCol = 1
retainCol = 3
idCol = 5

lookup = {}
encountered = []

createMarker = "id"
retainCounts = "mm"

for line in lines:
	if not (line.count(marker1) == 1 and line.count(marker2) == 1):
		continue
	i = line.index(marker1)
	j = line.index(marker2)
	if i >= j:
		continue
	mem = line[i+len(marker1):j].split(" ")
	
	memType = mem[typeCol]
	addr = mem[addrCol]
	memCount = int(mem[retainCol])
	
	if (encountered.count(addr) == 0):
		encountered.append(addr)
	
	if not lookup.has_key(memType):
		lookup[memType] = {}
	
	if (memCount > 0):
		if not lookup[memType].has_key(addr):
			createId = int(mem[idCol])
			lookup[memType][addr] = {createMarker : createId, retainCounts:[]}
		lookup[memType][addr][retainCounts].append(memCount)
	else:
		lookup[memType].pop(addr)
		encountered.remove(addr)

print "ITEMS STILL ALIVE"
for memType in lookup.keys():
	print memType
	
	for addr in encountered:
		if not lookup[memType].has_key(addr):
			continue
		
		memCounts = lookup[memType][addr][retainCounts]
		idx = len(memCounts) - 1
		if (memCounts[idx] == 0):
			continue
		print "    ", lookup[memType][addr][createMarker], addr, lookup[memType][addr][retainCounts]