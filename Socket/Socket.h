#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

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
        bool connect(const std::string& host, int port); // si connette ad un server

        bool send(const std::string& message) const; // invia un messaggio
        int receive(std::string& message) const; // riceve un messaggio

        void setNonBlocking(const bool); // setta il socket in modalità non bloccante
};

Socket::Socket() : m_sock(-1) { // inizializza il socket a -1
    memset(&m_addr, 0, sizeof(m_addr)); // inizializza l'indirizzo a 0
}

Socket::~Socket() { // distruttore per chiudere il socket
    if (m_sock != -1) { // se il socket è stato creato
        ::close(m_sock); // chiude il socket
    }
}

bool Socket::create() { // crea il socket
    m_sock = socket(AF_INET, SOCK_STREAM, 0); // crea un socket TCP
    if (m_sock == -1) { // se la creazione del socket fallisce
        return false; // ritorna false
    }

    int on = 1; // abilita l'opzione SO_REUSEADDR
    // imposta l'opzione SO_REUSEADDR per il socket m_sock
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        return false;
    }

    return true;
}

bool Socket::bind(int port) { // associa il socket ad una porta
    if (m_sock == -1) {
        return false;
    }

    m_addr.sin_family = AF_INET; // inizializzazione della famiglia dell'indirizzo a IPv4
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY); // inizializzazione dell'indirizzo ip del server
    m_addr.sin_port = htons(port); // inizializzazione della porta del server

    if (::bind(m_sock, (struct sockaddr*)&m_addr, sizeof(m_addr)) < 0) { // associa il socket ad un indirizzo e porta
        return false;
    }

    return true;
}

bool Socket::listen() const { // mette il socket in ascolto
    if (m_sock == -1) {
        return false;
    }

    if (::listen(m_sock, 1024) < 0) { // inizio dell'ascolto del socket
        return false;
    }

    return true;
}

bool Socket::accept(Socket& newSocket) const { // accetta una connessione in entrata
    int addr_length = sizeof(m_addr); // lunghezza dell'indirizzo
    newSocket.m_sock = ::accept(m_sock, (sockaddr*)&m_addr, (socklen_t*)&addr_length); // accetta la connessione in entrata e restituisce un nuovo socket

    if (newSocket.m_sock <= 0) { // se la connessione non è stata accettata
        return false;
    } else {
        return true;
    }
}

bool Socket::connect(const std::string& host, int port) { // si connette ad un server
    if (m_sock == -1) {
        return false;
    }

    m_addr.sin_family = AF_INET; // inizializzazione della famiglia dell'indirizzo a IPv4
    m_addr.sin_port = htons(port); // inizializzazione della porta del server


    if (inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr) <= 0) { // converte l'indirizzo ip da stringa a binario e lo memorizza in m_addr.sin_addr
        return false;
    }

    if (::connect(m_sock, (sockaddr*)&m_addr, sizeof(m_addr)) < 0) { // connessione al server 
        return false;
    }

    return true;
}

bool Socket::send(const std::string& message) const { // invia un messaggio
    if (::send(m_sock, message.c_str(), message.size(), MSG_NOSIGNAL) < 0) { // invia il messaggio attraverso il socket
        return false;
    } else {
        return true;
    }
}

int Socket::receive(std::string& message) const { // riceve un messaggio
    char buffer[1024]; // buffer per memorizzare il messaggio
    message.clear(); // pulisce il messaggio
    memset(buffer, 0, 1024); // inizializza il buffer a 0

    int status = ::recv(m_sock, buffer, 1024, 0); // riceve il messaggio attraverso il socket
    if (status == -1) { // se la ricezione del messaggio fallisce
        std::cout << "status == -1 errno == " << errno << " in Socket::recv\n";
        return 0;
    } else if (status == 0) { // se la connessione è stata chiusa
        return 0;
    } else { // se la ricezione del messaggio ha successo
        message = buffer; // memorizza il messaggio nel buffer
        return status; // ritorna la lunghezza del messaggio
    }
}

/*void Socket::setNonBlocking(const bool b) {
    int opts;

    opts = fcntl(m_sock, F_GETFL);

    if (opts < 0) {
        return;
    }

    if (b) {
        opts = (opts | O_NONBLOCK);
    } else {
        opts = (opts & ~O_NONBLOCK);
    }

    fcntl(m_sock, F_SETFL, opts);
}*/