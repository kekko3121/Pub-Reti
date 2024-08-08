#include <iostream>
#include <string>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include "Pub.h"
#include "../Socket/Socket.h"
using namespace std;

// Socket globale per il signal handling
Socket* serverSocketPtr = nullptr;
Pub* pub = nullptr; // Puntatore alla classe Pub per la memoria condivisa
int* clientiSeduti = nullptr; // Memoria condivisa per il contatore clientiSeduti

// Signal handler per l'interruzione del programma
void signalHandler(int);

int main() {
    int shmid; // Identificatore della memoria condivisa
    int clientiShmid; // Identificatore per la memoria condivisa dei contatori clientiSeduti
    Socket serverSocket; //dichiarazione della socket server


    // Creazione/accesso alla memoria condivisa
    if ((shmid = shmget(IPC_PRIVATE, sizeof(Pub), IPC_CREAT | 0666)) < 0) {
        perror("Errore durante la shmget per Pub");
        exit(EXIT_FAILURE);
    }

    // Attacco della memoria condivisa
    if ((pub = (Pub *)shmat(shmid, NULL, 0)) == (Pub *)-1) {
        perror("Errore durante la shmat per Pub");
        exit(EXIT_FAILURE);
    }

    // Creazione della memoria condivisa per i contatori clientiSeduti
    if ((clientiShmid = shmget(IPC_PRIVATE, sizeof(int) * 5, IPC_CREAT | 0666)) < 0) {
        perror("Errore durante la shmget per clientiSeduti");
        exit(EXIT_FAILURE);
    }

    // Attacco della memoria condivisa per i contatori clientiSeduti
    if ((clientiSeduti = (int*)shmat(clientiShmid, NULL, 0)) == (int*)-1) {
        perror("Errore durante la shmat per clientiSeduti");
        exit(EXIT_FAILURE);
    }

    // Inizializzazione del pub con 5 tavoli e un massimo di 20 posti
    new (pub) Pub(20, 5, clientiSeduti);

    //aggiunta dei tavoli
    pub->aggiungiTavolo(4);
    pub->aggiungiTavolo(6);
    pub->aggiungiTavolo(2);
    pub->aggiungiTavolo(5);
    pub->aggiungiTavolo(2);

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);
    serverSocketPtr = &serverSocket;

    // Creazione della socket server
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket!" << endl;
        return -1;
    }

    // Costruisce la socket con la porta 25564
    if (!serverSocket.bind(25564)) {
        cerr << "Errore nell'associazione del socket alla porta!" << endl;
        return -1;
    }

    // Imposta la socket in ascolto
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere il socket in ascolto!" << endl;
        return -1;
    }

    cout << "Il pub è aperto in attesa di clienti" << endl;

    while (true) { // ciclo per accettare più richieste client

        Socket clientSocket; //socket per acquisire le informazioni del client

        if (!serverSocket.accept(clientSocket)) { // accetta la connessione del client
            cerr << "Errore nell'accettare la connessione!" << endl;
            return -1;
        }

        cout << "Il cameriere sta servendo un Cliente" << endl; // stampa un messaggio di successo della connessione con il client

        // Crea un processo figlio per gestire la connessione client
        pid_t pid = fork();

        if(pid < 0){
            cerr << "Errore nella fork";
            continue;
        }

        if (pid == 0) { // Processo figlio

            string message; // memorizzo i messaggi che arrivano dal client

            if (clientSocket.receive(message) <= 0) { // se il messaggio non viene ricevuto correttamente
                break; // Connessione chiusa o errore ed esco dal ciclo
            }

            //verifica se ci sono posti disponibili per far accomodare il cliente
            if(message.compare("Ci sono posti liberi?") == 0){
                if(pub->postiDisponibili() > 0){
                    if (!clientSocket.send("Si")) {
                        break; // Errore nell'invio
                    }

                    message.clear();

                    if (clientSocket.receive(message) <= 0) { //ricevo dal cameriere la scelta del tavolo 
                        break;
                    }

                    //Se il cliente decide di accomodarsi ad un nuovo tavolo, si controlla se ci sono tavoli vuoti disponibili
                    if (message.compare("nuovo") == 0) {
                        int nuovoTavolo = pub->tavoloVuoto(); // memorizzo il numero di tavolo vuoto
                        if (nuovoTavolo > 0) { //Verifico se ci sono tavoli vuoti disponibili
                            if (pub->aggiungiCliente(nuovoTavolo)) { // Aggiungo il cliente al nuovo tavolo
                                cout << "Cliente aggiunto al tavolo nuovo: " << nuovoTavolo << endl;
                                clientSocket.send(to_string(nuovoTavolo)); // Invio il numero di tavolo al cameriere
                            } else {
                                cerr << "Errore: non è stato possibile aggiungere il cliente al tavolo nuovo: " << nuovoTavolo << endl;
                            }
                        } else {
                            clientSocket.send("Non ci sono tavoli vuoti"); // Avviso che non ci sono tavoli vuoti disponibili
                            break;
                        }
                    }
                    //Se il cliente decide di accomodarsi in un tavolo già occupato ma con posti disponibili
                    else{
                        try {
                            int ntavolo = stoi(message); //conversione della stinga per recuperare il numero di tavolo richiesto
                            if(pub->aggiungiCliente(ntavolo)){ //Aggiunge il cliente al tavolo se ci sono posti
                                clientSocket.send(message); //invia un messaggio di avviso al cameriere di posti disponibili
                            }
                            else{ //Se non ci sono posti
                                clientSocket.send("Tavolo inesistente o posti non disponibili"); //avvisa il camerire
                            }
                        //Eccezioni in caso di errore della conversione della stringa in intero
                        } catch (const invalid_argument& e) { 
                            clientSocket.send("Messaggio non valido: non è un numero");
                            break;
                        }
                    }

                    message.clear();

                    if (clientSocket.receive(message) <= 0) { // Ricevo l'ordine dal cameriere
                        break;
                    }
                    
                    if(message.substr(0, 14).compare("Prepara ordine") == 0){ //Se il cameriere ha consegnato l'ordine
                        pub->preparaOrdine(stoi(message.substr(15, 16))); //Prepara l'ordine per il tavolo indicato dal cameriere
                        //Il pub avvisa il camerire che può servire l'ordine
                        clientSocket.send("Ordine pronto al tavono n: " + message.substr(15, 16));
                    }

                    message.clear();
                    
                    //Il Cameriere avvisa che il cliente ha lasciato il locale
                    clientSocket.receive(message);

                    cout << "Cameriere: " << message << endl; //Stampa il messaggio del cameriere

                    message.clear();

                    /*if(message.substr(0,29).compare("Cliente ha liberato il tavolo") == 0){ //Se il cliente se ne è andato
                        pub->liberaPosto(stoi(message.substr(33))); //libero il posto al numero di tavolo
                        cout << "Si è liberato il posto al tavolo n: "  << message.substr(33) << endl; //stampo un avviso
                    }*/
                }
                //Avvisa il cameriere se nel Pub non ci sono posti
                else{
                    if (!clientSocket.send("No")) {
                        break; // Errore nell'invio
                    }
                }
            }

            shmdt(pub); // Detach shared memory in child process
            serverSocket.close();
            exit(0); // Termina il processo figlio
        }
    }

    // Dettach della memoria condivisa
    shmdt(pub);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

// Signal handler per l'interruzione del programma
void signalHandler(int signum) {
    if (serverSocketPtr != nullptr) { // se ho il contenuto della socket
        serverSocketPtr->close(); //chiudo la socket
        cout << "Server socket closed successfully." << endl; // Stampa di successo
    }
    exit(signum); // esco dal programma in base al codice passato
}