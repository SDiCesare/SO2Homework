# SO2Homework
Progetto di formattazione testo per l'esame Universitario di Sistemi Operativi 2.

# Compilazione
Il programma può essere compilato tramite il comando **make**, che genererà il file eseguibile **main**.

# Utilizzo
Il programma permette di formattare un file di testo UTF-8 in entrata in colonne separate, simile all'impaginazione di un giornale.
Si prevede in ingresso la larghezza, l'altezza, e il file di testo da formattare in input.
In aggiunta, si può eseguire il programma con le seguenti opzioni:

|Opzione            |Funzione                         |
|-------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|-s <int>           |Specifica il numero di spazi tra due colonne all'interno di una pagina. Il valore di default è 4.                                                                           |
|-c <int>           |Specifica il numero di colonne in una pagina. Il valore di default è 1.                                                                                                     |
|-o <file>          |Specifica il file in cui verrà scritto e salvato il nuovo testo. Il valore di default è "out.txt". Si può specificare come <file> "stdout" per stampare il testo su console.|
|-mp, --multiprocess|Esegue il programma in modalità multiprocesso.                                                                                                                              |
|-h, --help         |Visualizza le informazioni e le opzioni di utilizzo su console.                                                                                                             |

## Esempio di utilizzo
> ./main 20 7 canzone.txt
Il programma formatterà il testo all'interno del file "canzone.txt" con pagine da 1 colonna di dimensioni 20x7.

> ./main -c 5 15 5 canzone.txt
Il programma formatterà il testo all'interno del file "canzone.txt" con pagine da 5 colonna di dimensioni 15x5.

# Dettagli del Programma
Il programma si divide in 4 file:
- main.c
- multiprocess.h
- page.h
- util.h
Ogni file header ha un suo corrispettivo file ".c" che definisce le varie funzioni descritte e documentate nel suo header.

## main.c
Il punto iniziale del programma. Gestisce i dati in ingresso e si occupa della creazione e formattazione del testo in caso di esecuzione senza l'opzione -mp.

## multiprocess.h
Contiene le funzioni che permetto l'esecuzione del programma in versione multiprocesso. Contiene 3 funzioni, ognuna associata ad un processo:
- Il primo processo si occupa della lettura del file in parole, che verranno poi spedite al secondo.
- Il secondo processo riceve dal primo le singole parole, e si occupa della creazione della pagina passo passo.
- Infine il terzo processo, che riceve una pagina totalmente riempita dal secondo e effetta la formattazione e la stampa.
Mentre il processo padre aspetta che i figli finiscono, ed in caso di eventuali errori termina i vari figli e poi il se stesso. 

## page.h
Il file definisce le strutte dati Page, PageSection e PageCursor, le quali rappresentano e gestiscono una Pagina all'interno di un file di testo.
In oltre contiene tutte le funzioni per la creazione, l'inserimento e la serializzazione di una pagina.

## util.h
Il file serve a contenere delle generiche funzioni utili all'intero programma. Possiamo trovare il controllo della lunghezza di una stringa in UTF-8, e la scrittura in LITTLE INDIAN di valori interi
all'interno di un byte buffer.
