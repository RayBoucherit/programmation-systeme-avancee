CC = gcc
CFLAGS = -Wsizeof-pointer-memaccess -Wall -g
LDLIBS = -lrt -pthread
CLIENT = lpc_client.c
SERVEUR = lpc_serveur.c
FONCTION = lpc_fonction.c

client:
	$(CC) $(CFLAGS) $(LDLIBS) $(CLIENT) $(FONCTION) -o client

serveur:
	$(CC) $(CFLAGS) $(LDLIBS) $(SERVEUR) -o serveur	
clean: 
	rm serveur
	rm client