all:
	g++ -o Socket.o -c Socket.cpp
	g++ -o json.o -c json.cpp
	g++ -o fichierJSON.o -c fichierJSON.cpp
	g++ -o DMX.o -c DMX.cpp
	g++ -o enttecdmxusb.o -c enttecdmxusb.cpp
	g++ -o rs232.o -c rs232.cpp
	g++ -o testEnttecDMXUSBPRO.o -c testEnttecDMXUSBPRO.cpp
	g++ -o serveur Socket.o enttecdmxusb.o rs232.o testEnttecDMXUSBPRO.o DMX.o json.o fichierJSON.o
	./serveur
