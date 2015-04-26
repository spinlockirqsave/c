
/// @file:   README.txt
/// @author: peter cf16 eu
/// @date Apr 26, 2015, 02:57 PM

This file describes how to build and use sossrv and soscli programs
which are solutions of PiASK task 3 (Aplikacja kryzysowa dla ruchu morskiego).

FOLDER STRUCTURE
-README.txt	this file
-sos_common.h	shared definitions
-sossrv.c	server
-soscli.c	client

BUILD
Just type make which will run Makefile file to create both server (sossrv)
and client (soscli) in the same directory.

sossrv
Multithreaded server with max queue controlled by LISTENQ param and max of threads
by MAXTHREADS param (default is 4). Type ./sossrv to show complete synopsis.
Server opens the tcp port given as -p argument and listens for incomiing connections.
Server recognizes two messages: POS and SOS. Server handles SIGINT signal and prints
to stderr content of the list of ships stored in it's dynamic memory. Server keeps this
information persistently between different incoming connections through all it's run time.
usage:
./sossrv START -p port

POS message
Message used to signal a position of a ship. Server receives byte stream and interprets this
as a sos_ship structure that describes the ship and it's position. Server then updates 
appropriate entry in the list of ships or creates new entry in the list.

SOS message
Message sent to ask for help. Server extracts the ship position and calculates the distances
between this ship and all other ships stored in the list of ships (using the haversine formula,
www.movable-type.co.uk/scripts/latlong.html). Server then sorts the list of ships with respect
to the distance calculated in previous step and finally server sends this list to the client.

soscli
Client. Sends the POS and/or CLI messages to the sossrv server with:
./soscli POS -i ip -p port -n name -s signal -l longitude -a latitudde -v speed
(disconnects after message has been sent)
./soscli SOS -i ip -p port -n name -s signal -l longitude -a latitudde -v speed
(server waits for a list of sorted (by the distance) ships and prints that sorted
list to the stderr)

CLEAN
make clean
