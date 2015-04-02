//注意这个类的并行有问题，作者后来使用了python
#include <iostream>
#include <fstream>
#include "utils.cpp"
#include <string>
#include <map>
#include <omp.h>
#include <time.h>

using namespace std;

class Plist {
public:
	size_t frequency;
	size_t doc_id;

	Plist(size_t frequency, size_t doc_id) {
		this->frequency = frequency;
		this->doc_id = doc_id;
	}
};

class InvList {
public:

	map<string, vector<Plist> > plists;
	string prefix;
	InvList(string f, string prefix) {
		cout << "got filename:" << f << endl;
		this->prefix = prefix;
		int id;
		int doc_id = 0;
		vector<string> files = getData(f);
		vector<string>::iterator it;
		for (it = files.begin(); it != files.end(); it++) {
			cout << (*it) << endl;
		}
		map<string, vector<Plist> > plist = this->plists;

		int count = 0;
		clock_t start;
#pragma omp parallel for private(id,doc_id) shared(plist,start)
		// FIXME 这里没有等号，这样会导致少遍历一个document，已修正
		for (doc_id = 0; doc_id <= files.size() - 1; doc_id++) {
			id = omp_get_thread_num();
//				{
//	 			cout << "Thread = " << id << "with doc_id = " << doc_id << endl;
//	 			}
			this->process(files, plist, doc_id);

#pragma omp critical
			{
				count++;
				if (count % 100 == 0) {
					printf("Time elapsed: %f\n",
							((double) clock() - start) / CLOCKS_PER_SEC / 8);
					start = clock();
					cout << "clock = " << start << endl;
					cout << " 100 " << count << endl;
				}
			}
		}
		sort_and_write(plist);

	}
	//不同于utils.getFiles，该函数是从collection file中读取
	vector<string> getData(string f) {

		vector<string> files;
		string line;
		ifstream data(f.c_str());
		if (data.is_open()) {
			while (data.good()) {
				getline(data, line);
				// 	   cout << "adding files:" << line << endl;
				files.push_back(line);
			}
			data.close();
		} else
			cout << "Unable to open file";
		return files;
	}

	map<string, vector<Plist> > process(vector<string> files,
			map<string, vector<Plist> > &plist, int doc_id) {

		//cout << "received doc_id = " << doc_id << endl;
		string sfilter = " <>(),:.;\t\"\'!?-_\0\n[]=#{}";
		string line;
		int id;
		ifstream data;

#pragma omp critical
		{
			data.open(files[doc_id].c_str());
		}
		if (data.is_open()) {
#pragma omp private(data,line)
			{
				while (data.good()) {

					vector<string> terms;
					getline(data, line);
					Tokenize(line, terms, sfilter);
					size_t i;
#pragma omp private(terms,i) shared(plist)
					{
						for (i = 0; i < terms.size(); i++) {
							id = omp_get_thread_num();
							cout << "Thread = " << id << " with term = "
									<< terms[i] << endl;
#pragma omp critical
							{
								if (plist[terms[i]].size() == 0) {
									cout << "creating a fresh new vector for : "
											<< terms[i] << " in document = "
											<< doc_id << endl;
									plist[terms[i]].push_back(Plist(1, doc_id));
								} else if (plist[terms[i]][plist[terms[i]].size()
										- 1].doc_id != doc_id) {
									cout << "appending a new docid for : "
											<< terms[i] << "from<"
											<< plist[terms[i]][plist[terms[i]].size()
													- 1].doc_id << "> to <"
											<< doc_id << ">" << endl;
									plist[terms[i]].push_back(Plist(1, doc_id));
								} else {
									cout << "add frequency for: " << terms[i]
											<< "in " << doc_id << endl;
									size_t pos = plist[terms[i]].size();
									plist[terms[i]][pos - 1].frequency++;
								}
							}
						}

					}
				}
			}
#pragma omp barrier
			data.close();
		} else {
			cout << "error opening file" << endl;
		}

		return plist;
	}

	void Sort(vector<Plist>::iterator start, vector<Plist>::iterator end) {
		int numProcs = omp_get_num_procs();
		omp_set_num_threads(8);
#pragma omp parallel
		{
#pragma omp single nowait
			Sort_h(start, end - 1);
		}
	}

	void Sort_h(vector<Plist>::iterator start, vector<Plist>::iterator end) {
		if (start < end) {
			int q = Partition(start, end);
#pragma omp task
			{
				Sort_h(start, start + (q - 1));
			}
			Sort_h(start + (q + 1), end);
		}
	}

	int Partition(vector<Plist>::iterator start, vector<Plist>::iterator end) {
		int partitionIndex = (end - start) / 2;
		int privotValue = start[partitionIndex].frequency;
		Plist tmp = start[partitionIndex];
		start[partitionIndex] = *end;
		*end = tmp;
		int swapIndex = 0;
		for (int i = 0; i < end - start; i++) {
			if (start[i].frequency >= privotValue) {
				tmp = start[swapIndex];
				start[swapIndex] = start[i];
				start[i] = tmp;
				swapIndex++;
			}
		}
		tmp = start[swapIndex];
		start[swapIndex] = *end;
		*end = tmp;
		return swapIndex;
	}

	void sort_and_write(map<string, vector<Plist> > &plist) {
		cout << "Sorting..." << endl;
#pragma parallel for
		map<string, vector<Plist> >::iterator it;
		for (it = plist.begin(); it != plist.end(); it++) {
			Sort((*it).second.begin(), (*it).second.end());
			/*for (size_t i = 0 ;i < (*it).second.size();i++ )
			 {
			 for (size_t j = 0 ;j < (*it).second.size();j++ )
			 {
			 if ((*it).second[i].frequency > (*it).second[j].frequency )
			 {
			 Plist aux = (*it).second[i];
			 (*it).second[i] = (*it).second[j];
			 (*it).second[j] = aux;
			 }
			 }
			 }*/
		}

		stringstream words, invlist, invlistfreq;
		words << "../var/" << this->prefix << ".words";
		invlist << "../var/" << this->prefix << ".invlist";
		invlistfreq << "../var/" << this->prefix << ".invlistfreq";
		ofstream words_file(words.str().c_str());
		ofstream invlist_file(invlist.str().c_str());
		ofstream invlistfreq_file(invlistfreq.str().c_str());
		cout << "Storing Data..." << endl;
		for (it = plist.begin(); it != plist.end(); it++) {
			words_file << (*it).first << endl;
			invlist_file << (*it).second.size();
			invlistfreq_file << (*it).second.size();
			for (size_t i = 0; i < (*it).second.size(); i++) {
				Plist aux = (*it).second[i];
				invlist_file << " " << aux.doc_id;
				invlistfreq_file << " " << aux.frequency;
				//cout << "term = " << (*it).first << " doc_id = "
				//<< aux.doc_id << " frequency = " << aux.frequency << endl;
			}
			invlist_file << endl;
			invlistfreq_file << endl;
		}

		words_file.close();
		invlist_file.close();
		invlistfreq_file.close();
	}

};

//int main(int argc, char** argv) {
//	int cpus = atoi(argv[3]);
//	omp_set_num_threads(cpus);
//	const char* invlist = argv[1];
//	const char* prefix = argv[2];
//	InvList *inv = new InvList(string(invlist), string(prefix));
//
//}
