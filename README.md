# Pub-Reti

Progetto reti di calcolatori

![Logo](https://github.com/user-attachments/assets/089bcb1c-fe11-4b08-bbd6-214be888b38d)

## Traccia

Scrivere un'applicazione client/server parallelo in cui viene effettuata la gestione di un pub

C'è un solo cliente per tavolo

### Pub:

- Accetta un cliente se ci sono posti disponibili.
- Prepara gli ordini

### Cameriere:

- Chiede al pub se ci sono posti disponibili per un cliente
- Prende le ordinazioni e le invia al pub

### Cliente:

- Chiede se può entrare nel pub 
- Richiede al camerire il menu
- Effettua un ordine

Simulare la possibilità di avere più clienti al tavolo, ogni cliente è un nuovo client che si siede allo stesso tavolo (utilizzare un numero di tavolo). Ogni client effettua un ordine separato

## Run in locale

Scarica il progetto

```bash
  git clone https://github.com/kekko3121/Pub-Reti.git
```

Vai alla cartella del progetto

```bash
  cd /Pub-Reti
```

Installa le dipendenze

```bash
  gcc 
  g++
```

Compila il progetto:

- Pub:

```bash
  g++ Pub/main.cpp -o exe/Pub
```

- Cameriere:

```bash
  g++ Cameriere/main.cpp -o exe/Cameriere
```

- Cliente:

```bash
  g++ Cliente/main.cpp -o exe/Cliente
```

Esecuzione

```bash
  cd exe
  ./Pub
  ./Cameriere
  ./Cliente
```

## Documentazione

Per consultare la documentazione andare al seguente link: [https://github.com/kekko3121/Pub-Reti/blob/main/Relazione/Relazione_Progetto_Reti_di_Calcolatori___Laboratorio.pdf](https://github.com/kekko3121/Pub-Reti/blob/main/Relazione/Relazione_Progetto_Reti_di_Calcolatori___Laboratorio-2.pdf)

## Authors

- [@kekko3121](https://github.com/kekko3121)
- [@TonIann7](https://github.com/TonIann7)
