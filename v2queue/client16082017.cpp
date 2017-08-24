#include <iostream>
#include <fstream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <queue>

using namespace std;
using namespace zmqpp;
using namespace sf;

void messageToFile(const message& msg, const string& fileName) {
	const void *data;
	msg.get(&data, 1);
	size_t size = msg.size(1);

	ofstream ofs(fileName, ios::binary);
	ofs.write((char*)data, size);
}

void play()

void thread_function(){ //le paso el objeto music y le paso la cola
	while (music.getStatus() == SoundSource::Playing){
		cout << "suena!" << endl;
}

int main(int argc, char** argv) {
	cout << "This is the client\n";

	context ctx;
	socket s(ctx, socket_type::req);
	cout << "Connecting to tcp port 5555\n";
	s.connect("tcp://192.168.8.66:5555");

	queue<string> qsongs;// esta cola almacenara el nombre de las acciones que el usuario agrega

	Music music;

	while (true){

		cout << "operacion?" << endl;
		string operation;
		cin >> operation;

		message m;
		m << operation;

		if(operation == "play") {
			string file;
			cin >> file;
			m << file;
			// creo una cola donde almaceno el nombre de las canciones
			qsongs.push(file);
		}

		s.send(m);

		message answer;
		s.receive(answer);

		string result;
		answer >> result;
		if (result == "list") {
			size_t numSongs;
			answer >> numSongs;
			cout << "Available songs: " << numSongs << endl;
			for(int i = 0; i < numSongs; i++) {
				string s;
				answer >> s;
				cout << s << endl;
			}

		} else if (result == "file"){
			messageToFile(answer,"song.ogg");
			music.openFromFile("song.ogg");
			music.play();// es una operacion asincrona, se crea un hilo para reproducir song.ogg
			thread tControl(&thread_function, music, namesong);//prueba de hilo
			/*while (music.getStatus() == SoundSource::Playing){
				cout << "suena!" << endl;*/


			}
		} else {
			cout << "Don't know what to do!!!" << endl;
		}
	}
		return 0;
}
