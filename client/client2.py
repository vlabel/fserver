import socket
import struct
import threading
import Queue
import sys
import getopt
import os


class ClientCommand(object):
    CONNECT, SEND, RECEIVE, CLOSE = range(4)

    def __init__(self, type, data=None):
        self.type = type
        self.data = data


class ClientReply(object):
    ERROR, SUCCESS = range(2)

    def __init__(self, type, data=None):
        self.type = type
        self.data = data


class SocketClientThread(threading.Thread):
    def __init__(self, file, filename,  cmd_q=None):
        super(SocketClientThread, self).__init__()
        self.cmd_q = cmd_q or Queue.Queue()
        self.alive = threading.Event()
        self.alive.set()
        self.socket = None
        self.file = file
        self.filename = filename
        self.filename_gen = 0

        self.handlers = {
            ClientCommand.CONNECT: self._handle_CONNECT,
            ClientCommand.CLOSE: self._handle_CLOSE,
            ClientCommand.SEND: self._handle_SEND,
            ClientCommand.RECEIVE: self._handle_RECEIVE,
        }

    def run(self):
        while self.alive.isSet():
            try:
                # Queue.get with timeout to allow checking self.alive
                cmd = self.cmd_q.get(True, 0.1)
                self.handlers[cmd.type](cmd)
            except Queue.Empty as e:
                return

    def join(self, timeout=None):
        self.alive.clear()
        threading.Thread.join(self, timeout)

    def _handle_CONNECT(self, cmd):
        try:
            self.socket = socket.socket(
                socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((Config.TCP_IP, Config.TCP_PORT))
        except IOError as e:
            pass

    def _handle_CLOSE(self, cmd):
        self.socket.close()
        reply = ClientReply(ClientReply.SUCCESS)

    def _handle_SEND(self, cmd):
        filename_ending =  "" if self.filename_gen == 0 else str(self.filename_gen)
        self.filename_gen += 1
        message = self.filename + filename_ending +"\n"
        message += str(len(self.file)) + "\n"
        print 'Header      : "',message
        self.socket.send(message.encode('utf-8'))
        try:
            self.socket.sendall(self.file)
        except IOError as e:
            pass

    def _handle_RECEIVE(self, cmd):
        try:
            data = self.socket.recv(1024)
            print "Receive ",data
        except IOError as e:
            pass

    def _recv_n_bytes(self, n):
        data = ''
        while len(data) < n:
            chunk = self.socket.recv(n - len(data))
            if chunk == '':
                break
            data += chunk
        return data

    def _error_reply(self, errstr):
        return ClientReply(ClientReply.ERROR, errstr)

    def _success_reply(self, data=None):
        return ClientReply(ClientReply.SUCCESS, data)


class Config:
   inputfile = ''
   newname =  ''
   TCP_IP = '127.0.0.1'
   TCP_PORT = 3434
   clients = 1




def main(argv):
   try:
       opts, args = getopt.getopt(argv,"i:n:a:p:c:",["ifile=","newname="])
   except getopt.GetoptError:
       print 'client.py -i <inputfile> -n <newname> -h <host> -p <port> '
       sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print 'test.py -i <inputfile> -o <outputfile>'
         sys.exit()
      elif opt in ("-i", "--ifile"):
         Config.inputfile = arg
      elif opt in ("-n", "--newname"):
         Config.newname = arg
      elif opt in ("-h", "--host"):
         Config.TCP_IP = arg
      elif opt in ("-p", "--port"):
         Config.TCP_PORT = int(arg)
      elif opt in ("-c", "--clients"):
         Config.clients = int(arg)

   print 'Input file: ', Config.inputfile
   print 'Newname   : ', Config.newname
   print 'Host      : ', Config.TCP_IP
   print 'Port      : ', Config.TCP_PORT
   print 'Clients   : ', Config.clients

   if Config.newname == '':
       Config.newname = os.path.basename(Config.inputfile)

   f = open (Config.inputfile, "rb")
   wholefile = f.read()
   threads = []
   for j in range(Config.clients):
        q = Queue.Queue()
        for i in range(1000):
                q.put(ClientCommand(ClientCommand.CONNECT))
                q.put(ClientCommand(ClientCommand.SEND))
                q.put(ClientCommand(ClientCommand.RECEIVE))
                q.put(ClientCommand(ClientCommand.CLOSE))

        thread = SocketClientThread(wholefile, Config.newname+str(j), q)
        thread.start()
        threads.append(thread)

   while all([thread.isAlive() for thread in threads]):
       pass

if __name__ == "__main__":
   main(sys.argv[1:])
