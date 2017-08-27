#include <SFML/Audio.hpp>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <zmqpp/zmqpp.hpp>
#include <mutex>
#include <condition_variable>
#include "safequeue.h"

using namespace std;
using namespace zmqpp;
using namespace sf;

// template <class T, class Container = deque<T> >class queue;

void messageToFile(const message &msg, const string &fileName) {
  const void *data;
  msg.get(&data, 1);
  size_t size = msg.size(1);

  ofstream ofs(fileName, ios::binary);
  ofs.write((char *)data, size);
}

void songplay(Music *musicptr, SafeQueue<string> *qsongsptr, socket &s, bool &stop) {
  //cout <<endl<< "Inicia hilo songplay" << endl;
	while (!stop) {
    //while (qsongsptr->empty()) {}
    string songtoplay = qsongsptr->dequeue();
    //qsongsptr->pop();
    cout << "va a sonar:" << songtoplay << endl;
    message m;
		m << "play" << songtoplay;
    s.send(m);
    message answer;
    s.receive(answer);
    messageToFile(answer, "song.ogg");
    musicptr->openFromFile("song.ogg");
    musicptr->play();
    while (musicptr->getStatus() == SoundSource::Status::Playing && !stop) {}
  }
  return;
}

  void equeue(SafeQueue<string> *qsongseptr, string value){
    SafeQueue<string> auxqueue;
    string songtoerase;
    while(!qsongseptr->qempty()){
      songtoerase = qsongseptr->dequeue();
      if (songtoerase != value){
        auxqueue.enqueue(songtoerase);
      }
    }
    while(!auxqueue.qempty()){
      songtoerase = auxqueue.dequeue();
      qsongseptr->enqueue(songtoerase);
    }
  }
/*
void thread_function(songplay,){ //le paso el objeto music y le paso la cola
        while (music.getStatus() == SoundSource::Playing){
                cout << "suena!" << endl;*/

int main(int argc, char **argv) {
  cout << "\nThis is the client\n";

  context ctx;
  socket s(ctx, socket_type::req);
  cout << "Connecting to tcp port 5555\n";
  s.connect("tcp://localhost:5555");

  //std::queue<string> qsongs; // esta cola almacenara el nombre de las acciones
                             // que el usuario agrega
  SafeQueue<string> qsongs;
  Music music;
	bool stop = false;
  thread t(songplay, &music, &qsongs, std::ref(s), std::ref(stop));

  while (true) {
    cout <<endl<<"Operations available:"<<endl<<"list"<<endl<<"add"<<endl<<"play"<<endl<<"del"<<endl<<"stop o exit"<<endl;
    cout <<endl<<"Operacion?: ";
    string operation;
    cin >> operation;

    message m;
    // m << operation;

    if (operation == "play") {
      cout <<endl<< "Selecciono (play) digite el nombre de la cancion:";
      string file;
      cin >> file;
      qsongs.enqueue(file);
      //cout << "lo que tengo para play es:" << qsongs.front() << endl;
      //cout << "despues de t.join" << endl;
    } else if (operation == "list") {
      cout <<endl<< "Selecciono (list)" << endl;
      cout <<endl<< "Canciones del directorio (music): " <<endl<<endl;
      m << operation;
      s.send(m);
      message answer;
      s.receive(answer);

      size_t numSongs;
      string songL;

      answer >> numSongs;
      cout << "Available songs: " << numSongs << endl;
      for (int i = 0; i < numSongs; i++) {
        answer >> songL;
        cout << "Name song "<<(i+1)<<": "<< songL << endl;

      }
    } else if (operation == "add") {
      cout <<endl<< "Selecciono (add) digite el nombre de la cancion: ";
      string file;
      cin >> file;
      //cout << "si funciona" << qsongs.qempty() << endl;
      qsongs.enqueue(file);
      //cout << "si funciona2" << qsongs.qempty() << endl;
      /*
      int size;
      size = qsongs.sizequeue(qsongs);
      cout << "el tamaño de qsongs es" << size << endl;
      */
      //cout << "lo que tengo para add es:" << qsongs.front() << endl;
    }else if(operation == "del"){
      cout <<endl<< "Selecciono (del) digite el nombre de la cancion que desea borrar:";
      string element;
      cin >> element;
      equeue(&qsongs, element);
      cout <<endl<< "La cancion se elimino correctamente" << endl;

    } else if (operation == "stop") {
			stop = true;
      t.join();
      return 0;
    }else if (operation == "exit") {
      break;
    } else {
      cout << "Don't know what to do!!!" << endl;
    }
    /*
    if(operation !="add" || "list" || "play" ) {
                    cout << "Don't know what to do!!!" << endl;
            }*/
  }
  return 0;
}
