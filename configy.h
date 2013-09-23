#ifndef CONFIGY_INCLUDED_
#define CONFIGY_INCLUDED_

#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <map>
#include <list>

namespace configy {
	struct Collection {
		std::map<std::string, std::string> entries;
	};

	class Config {
	public:
		Config() {};
		~Config() {
			for (auto &t : this->cols) {
				delete t.second;
			}
		}
		void readFromFile(std::string file) {
			std::ifstream in(file, std::ios::in | std::ios::binary);
			if (in) {
				std::string contents;
				in.seekg(0, std::ios::end);
				contents.resize((unsigned int)in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&contents[0], contents.size());
				in.close();
				read(&contents);
			}
		}
		void readFromString(std::string* data) {
			read(data);
		}
		void getCollectionList(std::list<std::string>* list) {
			for (auto &t : this->cols)
				list->push_back(t.first);
		}
		bool getDataFromCollection(std::string str, std::map<std::string, std::string>* data) {
			try {
				*data = cols[str]->entries;
			} catch (const std::out_of_range& oor) {
				// Nothing to do?
				(oor);
				return false;
			}
			return true;
		}
	private:
		std::map<std::string, Collection*> cols;

		static void trim(std::string* str) {
			std::string::size_type pos = str->find_last_not_of(' ');
			if (pos != std::string::npos) {
				str->erase(pos + 1);
				pos = str->find_first_not_of(' ');
				if (pos != std::string::npos) str->erase(0, pos);
			} else str->erase(str->begin(), str->end());
		}

		static void removeComments(std::string* str) {
			int p;
			while (std::string::npos != (p = str->find('#'))) {
				int e;
				if (std::string::npos == (e = str->find('\n', p))) {
					e = str->length();
				}
				str->erase(p, e - p);
			}
		}

		void read(std::string* data) {
			std::istringstream s(*data);
			std::string line;
			std::string currentKey;
			this->cols[""] = new Collection();
			removeComments(data);

			while (std::getline(s, line)) {
				char c = line.at(0);
				int l = line.length();
				trim(&line);

				if (c == '[') {
					int o = line.find_first_of(']');
					try {
						currentKey = line.substr(1, o - 1);
					} catch (const std::out_of_range& oor) {
						// Nothing to do?
						(oor);
						continue;
					}
					trim(&currentKey);
					
					if (this->cols.find(currentKey) == this->cols.end()) {
						this->cols[currentKey] = new Collection();
					}
					continue;
				}

				if (('A' <= c) && (c <= 'Z') || ('a' <= c) && (c <= 'z')) {
					int eq;
					if ((eq = line.find('=')) != std::string::npos) {
						std::string a = line.substr(0, eq),
							b = line.substr(eq + 1, l - (eq + 2));
						trim(&a); trim(&b);
						this->cols[currentKey]->entries[a] = b;
					}
				}
			}
		}
	};
}

#endif
