#include <iostream>
#include <cstdlib>
#include <string>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
//#include <semaphore.h>
#include "Pub.h"
#include "../Socket/Socket.h"
using namespace std;

// Socket globale per il signal handling
Socket* serverSocketPtr = nullptr;
Pub* pub = nullptr; // Puntatore alla classe Pub per la memoria condivisa

// Signal handler per l'interruzione del programma
void signalHandler(int);

// Funzione del pub in caso di SIGNIT e liberare il tavolo
void termine_pub(Socket *, string *, int, Pub *);

int main() {
    int shmid; // Identificatore della memoria condivisa
    Socket serverSocket; // dichiarazione della socket server


    // Creazione/accesso alla memoria condivisa
    if ((shmid = shmget(IPC_PRIVATE, sizeof(Pub), IPC_CREAT | 0666)) < 0) {
        cerr << "Errore durante la shmget per Pub" << endl;
        exit(1); // In caso di errore esco dal programma
    }

    // Attacco della memoria condivisa
    if ((pub = (Pub *)shmat(shmid, NULL, 0)) == (Pub *)-1) {
        cerr << "Errore durante la shmat per Pub" << endl;
        exit(1);
    }

    // Inizializzazione del pub con 5 tavoli e un massimo di 20 posti
    new (pub) Pub(20, 5);

    // aggiunta dei tavoli
    if(!pub->aggiungiTavolo(4)){
        cerr << "Errore durante l'aggiunta dei tavoli";
    }

    if(!pub->aggiungiTavolo(6)){
        cerr << "Errore durante l'aggiunta dei tavoli";
    }

    if(!pub->aggiungiTavolo(2)){
        cerr << "Errore durante l'aggiunta dei tavoli";
    }

    if(!pub->aggiungiTavolo(5)){
        cerr << "Errore durante l'aggiunta dei tavoli";
    }

    if(!pub->aggiungiTavolo(2)){
        cerr << "Errore durante l'aggiunta dei tavoli";
    }

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);
    serverSocketPtr = &serverSocket; // passo l'indirizzo di memoria della variabile serverSocket

    // Creazione della socket server
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket!" << endl;
        exit(1);
    }

    // Costruisce la socket con la porta 25564
    if (!serverSocket.bind(25564)) {
        cerr << "Errore nell'associazione del socket alla porta!" << endl;
        exit(1);
    }

    // Imposta la socket in ascolto
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere il socket in ascolto!" << endl;
        exit(1);
    }

    cout << "Il pub è aperto in attesa di clienti" << endl; // Stampa di avviso

    while (true) { // ciclo per accettare più richieste client

        Socket clientSocket; // socket per acquisire le informazioni del client (cameriere)

        if (!serverSocket.accept(clientSocket)) { // accetta la connessione del client
            cerr << "Errore nell'accettare la connessione!" << endl;
            continue; // continua con il ciclo
        }

        cout << "Il cameriere sta servendo un Cliente" << endl; // stampa un messaggio di successo della connessione con il client

        // Crea un processo figlio per gestire la connessione client
        pid_t pid = fork();

        if(pid < 0) { // Errore creazione processo figlio
            cerr << "Errore nella fork";
            continue; // continua con il ciclo
        }

        if (pid == 0) { // Processo figlio

            string message; // memorizza i messaggi che arrivano dal client
            int ntavolo; // memorizza il numero di tavolo

            if (clientSocket.receive(message) <= 0) { // se il messaggio non viene ricevuto correttamente
                exit(1); //termino il processo figlio
            }

            //verifica se ci sono posti disponibili per far accomodare il cliente
            if(message.compare("Ci sono posti liberi?") == 0){
                if(pub->postiDisponibili() > 0){ // verifica disponibilità posti
                    if (!clientSocket.send("Si")) { // Invia si al cameriere se ci sono posti
                        exit(1); // Termina il processo figlio
                    }

                    message.clear();

                    if (clientSocket.receive(message) <= 0) { // riceve dal cameriere la scelta del tavolo 
                        exit(1); // Termina il processo figlio
                    }

                    // Se il cliente decide di accomodarsi ad un nuovo tavolo, si controlla se ci sono tavoli vuoti disponibili
                    if (message.compare("nuovo") == 0) {
                         ntavolo = pub->tavoloVuoto(); // memorizza il numero di tavolo vuoto
                        if (ntavolo > 0) { // Verifica se ci sono tavoli vuoti disponibili
                            if (pub->aggiungiCliente(ntavolo)) { // Aggiunge il cliente al nuovo tavolo
                                cout << "Cliente aggiunto al tavolo: " << ntavolo << endl;
                                clientSocket.send(to_string(ntavolo)); // Invia il numero di tavolo al cameriere
                            } else {
                                cerr << "Errore: non è stato possibile aggiungere il cliente al tavolo: " << ntavolo << endl;
                                exit(1); // Termina il processo figlio
                            }
                        } else {
                            clientSocket.send("Non ci sono tavoli vuoti"); // Avvisa che non ci sono tavoli vuoti disponibili
                            exit(1); // Termina il processo figlio
                        }
                    }

                    // Se il cliente decide di accomodarsi in un tavolo già occupato ma con posti disponibili
                    else{
                        try {
                            ntavolo = stoi(message); // conversione della stinga per recuperare il numero di tavolo richiesto
                            if(pub->aggiungiCliente(ntavolo)){ // Aggiunge il cliente al tavolo se ci sono posti
                                cout << "Cliente aggiunto al tavolo: " << ntavolo << endl;
                                clientSocket.send(message); // invia un messaggio di avviso al cameriere di posti disponibili
                            }
                            else{ //Se non ci sono posti
                                clientSocket.send("Tavolo inesistente o posti non disponibili"); // avvisa il camerire
                            }
                        // Eccezioni in caso di errore della conversione della stringa in intero
                        } catch (const invalid_argument& e) {
                            clientSocket.send("Messaggio non valido: non è un numero");
                            exit(1); // Termina il processo figlio
                        }
                    }

                    message.clear();

                    if (clientSocket.receive(message) <= 0) { // Ricevo l'ordine dal cameriere
                        exit(1); // Termina il processo figlio
                    }

                    termine_pub(&clientSocket, &message, ntavolo, pub); // Se il cliente o il cameriere si disconnettono senza preavviso
                    
                    if(message.substr(0, 14).compare("Prepara ordine") == 0){ // Se il cameriere ha consegnato l'ordine
                        pub->preparaOrdine(stoi(message.substr(15, 16))); // Prepara l'ordine per il tavolo indicato dal cameriere
                        // Il pub avvisa il camerire che può servire l'ordine
                        clientSocket.send("Ordine pronto al tavono n: " + message.substr(15, 16));
                    }

                    message.clear();
                }
                // Avvisa il cameriere se nel Pub non ci sono posti
                else{
                    if (!clientSocket.send("No")) {
                        exit(1); // Errore nell'invio
                    }
                }
            }

            message.clear();

            // Il Cameriere avvisa che il cliente ha lasciato il locale
            clientSocket.receive(message);

            cout << "Cameriere: " << message << endl; // Stampa il messaggio del cameriere

            message.clear();

            if(message.substr(0,29).compare("Cliente ha liberato il tavolo") == 0){ // Se il cliente se ne è andato
                pub->liberaPosto(stoi(message.substr(33))); // libera il posto al numero di tavolo
                cout << "Si è liberato il posto al tavolo n: "  << message.substr(33) << endl; // stampa un avviso
            }
            serverSocket.close(); // chiude la socket
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
        serverSocketPtr->send("termine_cameriere");
        serverSocketPtr->close(); //chiudo la socket
        cout << "Server socket closed successfully." << endl; // Stampa di successo
    }

    // Dettach della memoria condivisa
    shmdt(pub);
    exit(signum); // esco dal programma in base al codice passato
}

// Funzione specifica del pub per gestire i messaggi in caso di SIGNIT e liberare il tavolo
void termine_pub(Socket *client, string *message, int ntavolo, Pub *pub) {
    
    // Se il cameriere termina improvvisamente di funzionare
    if (message->compare("termine_cameriere") == 0) {
        cout << "Il cameriere si è disconnesso improvvisamente!" << endl; // Stampa di errore
        pub->liberaPosto(ntavolo); // libero il posto al numero di tavolo
        cout << "Si è liberato il posto al tavolo n: "  << ntavolo << endl; // Stampa di avviso
        client->close(); // chiusura della socket client
        exit(0); // Uscita dal programma
    }

    // Se il cliente termina improvvisamente di funzionare
    else if (message->compare("termine_cliente") == 0) {
        cout << "Un cliente si è disconnesso improvvisamente!" << endl; // Stampa di errore
        pub->liberaPosto(ntavolo); // libero il posto al numero di tavolo
        cout << "Si è liberato il posto al tavolo n: "  << ntavolo << endl; // Stampa di avviso
        exit(0); // Uscita dal programma
    }
}
