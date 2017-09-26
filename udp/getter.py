import socket



thisHIp = socket.gethostname()
thisHName = socket.gethostbyname(socket.gethostname())
PORT = 5800

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.bind((thisHIp, PORT)) #conecao para o transporte
        
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    # print '\n' data
    # print '\n' addr
    print "received message:", data