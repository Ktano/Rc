def readTxtFile(f):
	with open(filename, 'r+') as f:
		lineI = f.readlines()
		for i in range(0, len(lineI), 20):
			print lineI[i:i+20]
	
			for line in f:
				print line
				x += 1

	return 

# lower
# upper

filename = "input3.txt"
with open(filename, 'r+') as f:
	text = f.read()
	f.seek(0)
	f.write(text.lower())
f.close

readTxtFile(f)