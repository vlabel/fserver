import socket
import struct
import threading
import Queue
import sys
import getopt
import os
import signal


threads = []

class Config:
   inputfile = ''
   newname =  ''
   TCP_IP = '127.0.0.1'
   TCP_PORT = 3434
   clients = 1


def sig_handler(signum, frame):
    for thread in threads:
        thread.socket.shutdown(socket.SHUT_RDWR)
    pass


class ClientCommand(object):
    CONNECT, SEND, RECEIVE, CLOSE = range(4)

    def __init__(self, type ):
        self.type = type


class SocketClientThread(threading.Thread):
    BUFFER_SIZE = 1024 
    def __init__(self, file, filename,  cmd_q=None):
        super(SocketClientThread, self).__init__()
        self.cmd_q = cmd_q or Queue.Queue()
        self.alive = threading.Event()
        self.alive.set()
        self.socket = None
        self.file = file
        self.filename = filename

        self.handlers = {
            ClientCommand.CONNECT: self._handle_CONNECT,
            ClientCommand.CLOSE: self._handle_CLOSE,
            ClientCommand.SEND: self._handle_SEND,
            ClientCommand.RECEIVE: self._handle_RECEIVE,
        }

    def run(self):
        while self.alive.isSet():
            try:
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

    def _handle_SEND(self, cmd):
        message = self.filename + "\n"
        file_lenght = len(self.file) if self.file != '' else os.stat(Config.inputfile).st_size
        message += str(file_lenght) + "\n"
        print 'Sending file      : ', self.filename
        self.socket.send(message.encode('utf-8'))
        if self.file != '':
            try:
                self.socket.sendall(self.file)
            except IOError as e:
                pass
        else:
           f = open (Config.inputfile, "rb")
           print Config.inputfile
           l = f.read(self.BUFFER_SIZE)
           i = 0;
           Kb_sent = 0;
           try:
               while (l):
                   self.socket.send(l)
                   l = f.read(self.BUFFER_SIZE)
                   i+=1
                   if i == 100:
                       Kb_sent+=1
                       print "Sending ", Config.newname, " ",Kb_sent*100, " Kilobytes sent"
                       i = 0
           except socket.error as msg:
                print "Socket exception :",msg
                pass


    def _handle_RECEIVE(self, cmd):
        try:
            data = self.socket.recv(self.BUFFER_SIZE)
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


def main(argv):
   help_msg = 'client.py -i <inputfile> -n <newname> -h <host> -p <port> -c <clients> ' 
   try:
       opts, args = getopt.getopt(argv,"i:n:a:p:c:",["ifile=","newname="])
   except getopt.GetoptError:
       print help_msg
       sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print help_msg 
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

   wholefile = ''
   if Config.clients > 1:
       f = open (Config.inputfile, "rb")
       wholefile = f.read()
   global threads 
   for j in range(Config.clients):
        q = Queue.Queue()
        q.put(ClientCommand(ClientCommand.CONNECT))
        q.put(ClientCommand(ClientCommand.SEND))
        q.put(ClientCommand(ClientCommand.RECEIVE))
        q.put(ClientCommand(ClientCommand.CLOSE))
        file_ending = "" if j < 1 else str(j)
        thread = SocketClientThread(wholefile, Config.newname + file_ending, q)
        thread.start()
        threads.append(thread)

   while any([thread.isAlive() for thread in threads]):
       pass

if __name__ == "__main__":
   signal.signal(signal.SIGINT, sig_handler)
   main(sys.argv[1:])
