#!/usr/bin/python

#    build_lists.py
#    Copyright (C) 2011  Francisco Claude F.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
from operator import itemgetter, attrgetter
import re
import sys
# in ubuntu do 'sudo apt-get install python-nltk'
import nltk
import bisect
import getopt
import string
# in ubuntu do 'sudo apt-get install python-progressbar'
import progressbar


def usage():
    file = sys.argv[0]
    print("Usage:")
    print("\n")
    print(file+" -h")
    print("")
    print("\t shows this menu")
    print("\n")
    print(file+" -l <file_list> -w <output_words> -i <output_lists>")
    print("")
    print("\t reads a list of files from <file_list> and produces the")
    print("\t inverted lists in <output_lists>. The vocabulary is stored")
    print("\t in <output_words>. The program assumes that <file_list>")
    print("\t contains the file names sorted in alphabetical order (asc).")

pattern = re.compile('[\W_]+')
# leave only lowercase alpha-numeric symbols
def clean(str):
    return pattern.sub(' ',str.lower())

if len(sys.argv)==1:
    usage()
    sys.exit(0)

# Read options passed through the command line
try:
  optlist, args = getopt.getopt(sys.argv[1:],'hl:w:i:s')
except getopt.GetoptError, err:
    print(str(err))
    usage()
    sys.exit(1)

if ('h','') in optlist:
    usage()
    sys.exit(0)

sort = ""
print optlist
if ('-s','') in optlist:
  sort = "true"

# Assign names to the four files
files_list_name = ""
words_list_name = ""
inv_list_name = ""
doc_len_name = ""

for o,a in optlist:
    if o=="-l": files_list_name = a
    if o=="-w": words_list_name = a
    if o=="-i": inv_list_name = a


print("sort = "+sort)
if "" in [files_list_name,words_list_name,inv_list_name]:
    usage()
    sys.exit(0)

# Read files list
try:
    files_list_fp = open(files_list_name,"r")
    files = files_list_fp.readlines()
    files = map(lambda x: x.rstrip("\n"),files)
    files_list_fp.close()
except IOError as err:
    print(str(err))
    sys.exit(3)

stemmer = nltk.PorterStemmer()
#stemmer = nltk.SnowballStemmer("spanish")
words_list = {}
word_id = 0
words = []
lists = []
doc_len = []
doc_id = 0
total_docs = len(files)


widgets = ['Generating inverted lists: ', progressbar.Percentage(),'     ',progressbar.ETA()]
progress,c = progressbar.ProgressBar(widgets=widgets, maxval = len(files)),1
for f_name in files:
    try:
        f_fp = open(f_name,"r")
        content = clean(f_fp.read())
        tokens = nltk.WordPunctTokenizer().tokenize(content)
        doc_len.append(len(tokens))
        for w in tokens:
            if len(w)>32: continue
            try:
                word = stemmer.stem(w)
            except:
                word = w
            if word not in words_list:
                words_list[word] = word_id
                words.append((word,word_id))
                word_id+=1
                lists.append([])
            act_id = words_list[word]
            lenl = len(lists[act_id])
            if lenl==0 or lists[act_id][lenl-1][0]!=doc_id:
                lists[act_id].append([doc_id,1])
  	    else:
	        lists[act_id][lenl-1] = [doc_id,lists[act_id][lenl-1][1] + 1]
        f_fp.close()
        progress.update(c)
        c+=1
        doc_id += 1
    except IOError as err:
        print(str(err))
        sys.exit(4)
progress.finish()

words.sort()
perm = [0]*len(words)
c = 0
for w,i in words:
    perm[c] = i
    c += 1
	
widgets = ['Storing data: ', progressbar.Percentage(),'     ',progressbar.ETA()]
progress,c = progressbar.ProgressBar(widgets=widgets, maxval = len(lists)),1
try:
    words_list_fp = open(words_list_name+".words","w")
    for w in words:
        words_list_fp.write(w[0]+"\n")
    words_list_fp.close()
    doc_len_fp = open(inv_list_name+".doclen","w")
    doc_len_fp.write("%d\n"%total_docs)
    for i in range(0,len(doc_len)):
        doc_len_fp.write("%d "%i)
        doc_len_fp.write("%d"%doc_len[i])
        doc_len_fp.write("\n")
    doc_len_fp.close()

    inv_list_fp = open(inv_list_name+".invlist","w")
    inv_list_freq_fp = open(inv_list_name+".invlistfreq","w")
    for p in perm:
        l = lists[p]
        inv_list_fp.write("%d "%len(l))
      	inv_list_freq_fp.write("%d "%len(l))
        if sort == "true":
            k = sorted(lists[p] ,key=itemgetter(1) , reverse=True)
        else:
            k = lists[p]
        for (id,count) in k:
            inv_list_fp.write("%d "%id)
      	    inv_list_freq_fp.write("%d "%count)

        inv_list_freq_fp.write("\n")
        inv_list_fp.write("\n")
        progress.update(c)
        c += 1
    inv_list_fp.close()
    inv_list_freq_fp.close()
except IOError as err:
    print(str(err))
    sys.exit(4)
progress.finish()
