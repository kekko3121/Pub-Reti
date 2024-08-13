#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

class Socket {
    private:
        int m_sock; // descrizione del socket
        sockaddr_in m_addr; // indirizzo del socket (IP e porta)

    public:
        Socket(); // costruttore per inizializzare il socket
        ~Socket(); // distruttore per chiudere il socket

        bool create(); // crea il socket
        bool bind(int port); // associa il socket ad una porta
        bool listen() const; // mette il socket in ascolto
        bool accept(Socket& newSocket) const; // accetta una connessione in entrata
        bool connect(const string& host, int port); // si connette ad un server
        bool close(); // chiude il socket

        bool send(const string& message) const; // invia un messaggio
        int receive(string& message) const; // riceve un messaggio

        void setNonBlocking(const bool); // setta il socket in modalità non bloccante
};

Socket::Socket() : m_sock(-1) { // inizializza il socket a -1
    memset(&m_addr, 0, sizeof(m_addr)); // inizializza l'indirizzo a 0
}

Socket::~Socket() { // distruttore per chiudere il socket
    close(); // ensure the socket is closed
}

bool Socket::create() { // crea il socket
    m_sock = socket(AF_INET, SOCK_STREAM, 0); // crea un socket TCP
    if (m_sock == -1) { // se la creazione del socket fallisce
        cerr << "Errore nella creazione del socket: " << strerror(errno) << endl;
        return false; // ritorna false
    }

    int on = 1; // abilita l'opzione SO_REUSEADDR
    // imposta l'opzione SO_REUSEADDR per il socket m_sock
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        cerr << "Errore nell'impostazione delle opzioni del socket: " << strerror(errno) << endl;
        return false;
    }

    return true;
}

bool Socket::bind(int port) { // associa il socket ad una porta
    if (m_sock == -1) {
        cerr << "Socket non valido. Creare il socket prima di fare il bind." << endl;
        return false;
    }

    m_addr.sin_family = AF_INET; // inizializzazione della famiglia dell'indirizzo a IPv4
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY); //Imposta l'indirizzo IP a INADDR_ANY per accettare connessioni da qualsiasi interfaccia di rete.
    m_addr.sin_port = htons(port); // Converte la porta dal formato dell'host a quello della rete.

    if (::bind(m_sock, (struct sockaddr*)&m_addr, sizeof(m_addr)) < 0) { // associa il socket ad un indirizzo e porta
        cerr << "Errore nel bind del socket: " << strerror(errno) << endl;
        return false;
    }

    return true;
}

bool Socket::listen() const { // mette il socket in ascolto
    if (m_sock == -1) {
        cerr << "Socket non valido. Creare e fare il bind del socket prima di metterlo in ascolto." << endl;
        return false;
    }

    if (::listen(m_sock, 1024) < 0) { // inizio dell'ascolto del socket
        //il secondo parametro \texttt{1024} rappresenta il numero massimo di connessioni in coda che possono essere gestite.
        cerr << "Errore nell'ascolto del socket: " << strerror(errno) << endl;
        return false;
    }

    return true;
}

bool Socket::accept(Socket& newSocket) const { // accetta una connessione in entrata
    if (m_sock == -1) {
        cerr << "Socket non valido. Creare, fare il bind e mettere in ascolto il socket prima di accettare connessioni." << endl;
        return false;
    }

    socklen_t addr_length = sizeof(m_addr); //Determina la dimensione della struttura che conterrà l'indirizzo del client.
    newSocket.m_sock = ::accept(m_sock, (sockaddr*)&m_addr, &addr_length); // accetta la connessione in entrata e restituisce un nuovo socket per gestirla

    if (newSocket.m_sock <= 0) { // se la connessione non è stata accettata
        cerr << "Errore nell'accettare la connessione: " << strerror(errno) << endl;
        return false;
    } else {
        return true;
    }
}

bool Socket::connect(const string& host, int port) { // si connette ad un server
    if (m_sock == -1) {
        cerr << "Socket non valido. Creare il socket prima di connettersi." << endl;
        return false;
    }

    m_addr.sin_family = AF_INET; // inizializzazione della famiglia dell'indirizzo a IPv4
    m_addr.sin_port = htons(port); // inizializzazione della porta del server

    if (inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr) <= 0) { // converte l'indirizzo ip da stringa a binario e lo memorizza in m_addr.sin_addr
        cerr << "Errore nella conversione dell'indirizzo IP: " << strerror(errno) << endl;
        return false;
    }

    if (::connect(m_sock, (sockaddr*)&m_addr, sizeof(m_addr)) < 0) { // connessione al server 
        cerr << "Errore nella connessione al server: " << strerror(errno) << endl;
        return false;
    }

    return true;
}

bool Socket::send(const string& message) const { // invia un messaggio
    if (m_sock == -1) {
        cerr << "Socket non valido. Creare il socket prima di inviare messaggi." << endl;
        return false;
    }

    if (::send(m_sock, message.c_str(), message.size(), MSG_NOSIGNAL) < 0) { // invia il messaggio attraverso il socket
        cerr << "Errore nell'invio del messaggio: " << strerror(errno) << endl;
        return false;
    } else {
        return true;
    }
}

int Socket::receive(string& message) const { // riceve un messaggio
    if (m_sock == -1) {
        cerr << "Socket non valido. Creare il socket prima di ricevere messaggi." << endl;
        return -1;
    }

    char buffer[1024]; // buffer per memorizzare il messaggio
    message.clear(); // pulisce il messaggio
    memset(buffer, 0, sizeof(buffer)); // inizializza il buffer a 0

    int status = ::recv(m_sock, buffer, sizeof(buffer), 0); // riceve il messaggio attraverso il socket
    if (status < 0) { // se la ricezione del messaggio fallisce
        cerr << "Errore nella ricezione del messaggio: " << strerror(errno) << endl;
        return -1;
    } else if (status == 0) { // se la connessione è stata chiusa
        cerr << "Connessione chiusa dal peer." << endl;
        return 0;
    } else { // se la ricezione del messaggio ha successo
        message.assign(buffer, status); // assegna il messaggio del buffer
        return status; // ritorna la lunghezza del messaggio
    }
}

bool Socket::close() { //chiude la socket
    //Tenta di chiudere il socket usando la funzione di sistema
    if (::close(m_sock) < 0) { //se la chiusura del socket fallisce
        cerr << "Errore nella chiusura del socket: " << strerror(errno) << endl;
        return false;
    }

    m_sock = -1; // Ripristina la descrizione del socket

    // Controllare se ci sono stati errori specifici durante la chiusura
    if (errno == EINTR) {
        cerr << "Chiusura del socket interrotta da un segnale." << endl;
    } else if (errno == EBADF) {
        cerr << "Descrizione del socket non valida." << endl;
    } else if (errno == EIO) {
        cerr << "Errore I/O durante la chiusura del socket." << endl;
    }

    return true;
}

void Socket::setNonBlocking(const bool b) { // setta il socket in modalità non bloccante
    int opts = fcntl(m_sock, F_GETFL); // recupera le opzioni del socket

    if (opts < 0) { //se non riesco a leggere le impostazioni del socket
        cerr << "Errore nel recupero delle opzioni del socket: " << strerror(errno) << endl;
        return;
    }

    if (b) { // se b è vero 
        opts |= O_NONBLOCK; // setta il socket in modalita non bloccante
    }
    
    else {
        opts &= ~O_NONBLOCK; // setta il socket in modalità bloccante (rimuovo l'opzione)
    }

    if (fcntl(m_sock, F_SETFL, opts) < 0) { // Applica le nuove impostazioni al socket
        cerr << "Errore nell'impostazione delle opzioni del socket: " << strerror(errno) << endl;
    }
}
