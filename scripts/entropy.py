from __future__ import division
from math import log
from sys import argv

if __name__ == "__main__":
  f = open(str(argv[0]))
  freq = {}
  count = 0 
  for line in f:
    linei = line.split(" ")
    for l in linei:
      if l not in freq:
        freq[l] = 1
        count = count + 1
      else:
        freq[l] = freq[l] + 1
        count = count + 1
  entropy = 0.0
  for key in freq.keys():
    p = freq[key]/count
    entropy = entropy + -p*(log(p,2))
  print entropy

