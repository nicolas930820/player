#include <iostream>
#include <fstream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace zmqpp;
using namespace sf;

/*
calificacion por la sustentacion y no por la entrega


#examinar directorio para cargar todo los ogg y filtrar si es posible

#lista de reproducciones
addicionar
borrar cancion

#interaccion  cliente
next
play

round robin balanceo de cargas

archivos no existentes
archivos enviados para varios clientes 'no hilos'
archivos solo .ogg

enviar conexiones por partes 512k
*/

void messageToFile(const message& msg, const string& fileName) {
	const void *data;
	msg.get(&data, 1);
	size_t size = msg.size(1);

	ofstream ofs(fileName, ios::binary);
	ofs.write((char*)data, size);
}

int main(int argc, char** argv) {
	cout << "This is the client\n";

	context ctx;
	socket s(ctx, socket_type::req);
	cout << "Connecting to tcp port 5555\n";
	s.connect("tcp://192.168.8.66:5555");

	Music music;

	cout << "Sending  some work!\n";

	message m;
	string operation(argv[1]);

	m << operation;

	if(argc == 3) {
		string file(argv[2]);
		m << file;
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
		int x;
		cin >> x;// espera que el cliente ingrese algo para retornar el cero de mas abajo, si no se crea el hilo principal se va a terminal y los hilos creados a partir de el principal se terminaran y no se va a escuchar toda la cancion
	} else {
		cout << "Don't know what to do!!!" << endl;
	}

	return 0;
}
