#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

using namespace std;
struct eqstr
{
  bool
  operator() (const char* s1, const char* s2) const
  {
    return (s1 == s2) || (s1 && s2 && strcmp (s1, s2) == 0);
  }
};
// 根据输入的目录名，获取该目录下第一层所包含的非目录文件名
vector<string>
getFiles (string dir)
{
  vector<string> files;
  struct dirent *de = NULL;
  DIR *d = NULL;
  const char* dirc = dir.c_str ();
  d = opendir (dirc);
  if (d == NULL)
    {
      perror ("Couldn't open directory");
    }
  while (de = readdir (d))
    {
	  cout<<de->d_name<<endl;
      if (de->d_type != DT_DIR)
	files.push_back ((string) de->d_name);
    }
  closedir (d);
  return files;
}

void
Tokenize (const string& str, vector<string>& tokens,
	  const string& delimiters = " ")
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of (
      delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos = str.find_first_of (delimiters,
					     lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back (
	  str.substr (lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of (delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of (delimiters, lastPos);
    }
}

void
printBits (uint v)
{
  for (int i = sizeof(uint) * 8 - 1; i >= 0; i--)
    {
      int bit = ((v >> i) & 1);
      cout << bit;
    }
  cout << endl;
}

void
printVectorS (vector<string> v)
{
  for (int i = 0; i < v.size (); i++)
    {
      cout << v[i] << endl;
    }
}

void
printVectorI (vector<int> v)
{
  for (int i = 0; i < v.size (); i++)
    {
      cout << v[i] << endl;
    }
}

unsigned char
int2char (int n)
{
  return n + 0x30;
}

int
rank1 (vector<int> x, int i)
{
  cout << "rank i = " << i << endl;
  int count = 0;
  for (int j = 0; j < i; j++)
    {
      if (x[j] == 1)
	count++;
    }
  return count;
}

int
select1 (vector<int> x, int i)
{
  int count = 0;
  for (int j = 0; j < x.size (); j++)
    {
      if (x[j] == 1)
	count++;
      if (count == i)
	return j + 1;
    }
}

void
printVectorU (vector<uint> v)
{
  for (int i = 0; i < v.size (); i++)
    {
      cout << v[i] << endl;
    }
}

string stopwords[320] =
  { "a", "about", "above", "above", "across", "after",
      "afterwards", "again", "against", "all", "almost",
      "alone", "along", "already", "also", "although",
      "always", "am", "among", "amongst", "amoungst",
      "amount", "an", "and", "another", "any", "anyhow",
      "anyone", "anything", "anyway", "anywhere", "are",
      "around", "as", "at", "back", "be", "became",
      "because", "become", "becomes", "becoming", "been",
      "before", "beforehand", "behind", "being", "below",
      "beside", "besides", "between", "beyond", "bill",
      "both", "bottom", "but", "by", "call", "can",
      "cannot", "cant", "co", "con", "could", "couldnt",
      "cry", "de", "describe", "detail", "do", "done",
      "down", "due", "during", "each", "eg", "eight",
      "either", "eleven", "else", "elsewhere", "empty",
      "enough", "etc", "even", "ever", "every", "everyone",
      "everything", "everywhere", "except", "few",
      "fifteen", "fify", "fill", "find", "fire", "first",
      "five", "for", "former", "formerly", "forty", "found",
      "four", "from", "front", "full", "further", "get",
      "give", "go", "had", "has", "hasnt", "have", "he",
      "hence", "her", "here", "hereafter", "hereby",
      "herein", "hereupon", "hers", "herself", "him",
      "himself", "his", "how", "however", "hundred", "ie",
      "if", "in", "inc", "indeed", "interest", "into", "is",
      "it", "its", "itself", "keep", "last", "latter",
      "latterly", "least", "less", "ltd", "made", "many",
      "may", "me", "meanwhile", "might", "mill", "mine",
      "more", "moreover", "most", "mostly", "move", "much",
      "must", "my", "myself", "name", "namely", "neither",
      "never", "nevertheless", "next", "nine", "no",
      "nobody", "none", "noone", "nor", "not", "nothing",
      "now", "nowhere", "of", "off", "often", "on", "once",
      "one", "only", "onto", "or", "other", "others",
      "otherwise", "our", "ours", "ourselves", "out",
      "over", "own", "part", "per", "perhaps", "please",
      "put", "rather", "re", "same", "see", "seem",
      "seemed", "seeming", "seems", "serious", "several",
      "she", "should", "show", "side", "since", "sincere",
      "six", "sixty", "so", "some", "somehow", "someone",
      "something", "sometime", "sometimes", "somewhere",
      "still", "such", "system", "take", "ten", "than",
      "that", "the", "their", "them", "themselves", "then",
      "thence", "there", "thereafter", "thereby",
      "therefore", "therein", "thereupon", "these", "they",
      "thickv", "thin", "third", "this", "those", "though",
      "three", "through", "throughout", "thru", "thus",
      "to", "together", "too", "top", "toward", "towards",
      "twelve", "twenty", "two", "un", "under", "until",
      "up", "upon", "us", "very", "via", "was", "we",
      "well", "were", "what", "whatever", "when", "whence",
      "whenever", "where", "whereafter", "whereas",
      "whereby", "wherein", "whereupon", "wherever",
      "whether", "which", "while", "whither", "who",
      "whoever", "whole", "whom", "whose", "why", "will",
      "with", "within", "without", "would", "yet", "you",
      "your", "yours", "yourself", "yourselves", "the" };

bool
isStopWord (string t)
{
  for (int i = 0; i < 319; i++)
    {
      if (stopwords[i] == t)
	return 1;
    }
  return 0;
}

