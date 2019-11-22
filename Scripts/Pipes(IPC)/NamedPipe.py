import os, sys
# Path to be created
path = "./namedpipe"
try:
    os.mkfifo(path)
except OSError, e:
    print("Falla al crear FIFO: %s" % e)
else:
    fifo = open(path, 'w')
print "Path fue creado."