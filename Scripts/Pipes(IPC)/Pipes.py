#!/usr/bin/python

import os, sys

print("El proceso hijo escribira algo ")
print("y el proceso padre leera el texto escrito por el proceso hijo...")

# descriptores r y w para lectura y escritura
r, w = os.pipe() 

processid = os.fork()
if processid:
   # Este corresponde al proceso padre 
   # Cierra el descriptor w
   os.close(w)
   r = os.fdopen(r)
   print("Proceso padre lee")
   str = r.read()
   print("texto =", str)   
   sys.exit(0)
else:
   # This is the child process
   os.close(r)
   w = os.fdopen(w, 'w')
   print("Proceso hijo escribiendo")
   w.write("Python es horroroso.")
   w.close()
   print("Proceso hijo cerrando")
   sys.exit(0)