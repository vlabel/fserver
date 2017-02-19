import socket
import select

import sys, getopt, os

def main(argv):
   inputfile = ''
   newname =  '' 
   TCP_IP = '127.0.0.1'
   TCP_PORT = 3434
   try:
       opts, args = getopt.getopt(argv,"i:n:a:p:",["ifile=","newname="])
   except getopt.GetoptError:
       print 'client.py -i <inputfile> -n <newname> -h <host> -p <port> '
       sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print 'test.py -i <inputfile> -o <outputfile>'
         sys.exit()
      elif opt in ("-i", "--ifile"):
         inputfile = arg
      elif opt in ("-n", "--newname"):
         newname = arg
      elif opt in ("-h", "--host"):
         TCP_IP = arg
      elif opt in ("-p", "--port"):
         TCP_PORT = int(arg)

   print 'Input file: ', inputfile
   print 'Newname   : ', newname
   print 'Host      : ', TCP_IP
   print 'Port      : ', TCP_PORT

   if newname == '':
       newname = os.path.basename(inputfile)
   BUFFER_SIZE = 1024

   s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   s.connect((TCP_IP, TCP_PORT))
   message = newname+"\n"
   size = os.stat(inputfile).st_size
   message += str(size) + "\n"
   print 'Header      : "',message 
   s.send(message)
   f = open (inputfile, "rb")
   l = f.read(BUFFER_SIZE)
   i = 0;
   Kb_sent = 0;
   try:
       while (l):
           s.send(l)
           l = f.read(BUFFER_SIZE)
           i+=1
           if i == 100:
               Kb_sent+=1
               print "Sending ", newname, " ",Kb_sent*100, " Kilobytes sent"
               i = 0
   except socket.error as msg:
        print "Socket exception :",msg
        pass
   data = s.recv(BUFFER_SIZE)
   print "received data:", data
   print data
   s.close()



if __name__ == "__main__":
   main(sys.argv[1:])
