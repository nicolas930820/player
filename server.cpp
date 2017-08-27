#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include <glob.h>

//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:usr/local/lib

using namespace std;
using namespace zmqpp;

vector<char> readFileToBytes(const string& fileName) {
	ifstream ifs(fileName, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	vector<char> result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read(result.data(), pos);

	return result;
}

void fileToMesage(const string& fileName, message& msg) {
	vector<char> bytes = readFileToBytes(fileName);
	msg.add_raw(bytes.data(), bytes.size());
}

// Split all available songs
vector<string> split(string s, char del){
    vector<string> v;
    string nameSong = "";

    for (int i = 0; i < int(s.size()); i++) {
        if (s[i] != del)
            nameSong += s[i];

        else{
            v.push_back(nameSong);
            nameSong = "";
        }
    }

    if (nameSong != "")
        v.push_back(nameSong);

    return v;
}

// Load all files from directory
unordered_map<string,string> fromDirectory(const string& path){
    glob_t glob_result;
    glob(path.c_str(), GLOB_TILDE, NULL, &glob_result);

    vector<string> files;

    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
        files.push_back(string(glob_result.gl_pathv[i]));

    globfree(&glob_result);
    unordered_map<string,string> Result;

    for (int i = 0; i < int(files.size()); i++){
        vector<string> vec = split(files[i], '.');

        if (vec[1] == "ogg"){
            vec = split(vec[0], '/');
            Result[vec[1]] = files[i];
        }
    }

    return Result;
}

int main(int argc, char** argv) {
  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  // string path(argv[1]);
  // unordered_map<string,string> songs;
  // songs["s1"] = path + "s1.ogg";
  // songs["s2"] = path + "s2.ogg";
  // songs["s3"] = path + "s3.ogg";
	string path(argv[1]);
	unordered_map<string,string> songs;
	songs = fromDirectory(path + "*");

  cout << "Start serving requests!\n";
  while(true) {
    message m;
    s.receive(m);

    string op;
    m >> op;

    cout << "Action:  " << op << endl;
    if (op == "list") {  // Use case 1: Send the songs
      message n;
      n << songs.size();
      for(const auto& p : songs)
        n << p.first;
      s.send(n);
    } else if(op == "play") {
      // Use case 2: Send song file
      string songName;
      m >> songName;
      cout << "sending song " << songName
           << " at " << songs[songName] << endl;
			message n;
			n << "file";
			fileToMesage(songs[songName], n);
			s.send(n);
    } else {
      cout << "Invalid operation requested!!\n";
    }
  }

  cout << "Finished\n";
  return 0;
}
