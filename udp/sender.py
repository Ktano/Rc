import socket

H_NAME = "lab6p8"
PORT = 5800
MESSAGE = "Wubba!"
server_h_adr = socket.gethostbyname(H_NAME)

print "lets do it:\n"

print "UDP target:"
print "IP:", server_h_adr
print "port:", PORT
print "message:", MESSAGE

sock = socket.socket(socket.AF_INET,
	socket.SOCK_DGRAM)

sock.sendto(MESSAGE, (server_h_adr	, PORT))