from sys import argv
import random

if __name__ == "__main__":
	n = int(argv[1])
	k = int(argv[2])
	s = int(argv[5])
	vocabulary = open(argv[3],'r')
	ps = open(argv[4],'r')
	prefix = str(argv[6])
	words = []
	query = []
	for line in vocabulary:
#		if i % 1000 == 0:
#			print i
		words.append(line[0:-1])
	i = 0
	for line in ps:
		psl = line.split(" ")[0]
		if (int(psl) >= s):
			query.append(words[i])		
		i+=1
	
	for k in range(2,11):
		queryfile = open(prefix+str(k),'w')
		queryfile.write(str(n)+"\n")
		for j in range(0,int(n)):
			for i in range(0,int(k)):
				queryfile.write(str(query[random.randint(0,len(query)-1)])+" ")
			queryfile.write("\n")
		queryfile.close()

	
