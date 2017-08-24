#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

int main(int argc, char** argv) {
	cout << "This is the client\n";

	context ctx;
	socket s(ctx, socket_type::req);

	cout << "Connecting to tcp port 5555\n";
	s.connect("tcp://localhost:5555");

	cout << "Sending  some work!\n";

	message m;
	string operation(argv[1]);

	m << operation;
	s.send(m);

	message answer;
	s.receive(answer);

	string result;
	answer >> result;
	if (result == "list") {
		size_t numSongs;// size_t es la entrada en la hastable
		answer >> numSongs;
		cout << "Available songs: " << numSongs << endl;
		for(int i = 0; i < numSongs; i++) {
			string s;
			answer >> s;
			cout << s << endl;
		}

	} else {

	}

	return 0;
}
