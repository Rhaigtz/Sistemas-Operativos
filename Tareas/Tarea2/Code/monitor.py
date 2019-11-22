from threading import Thread, Semaphore
from time import sleep, time
from random import randrange
buff=5
cap=10
class productor(Thread):
	def __init__(self, mutex):
		Thread.__init__(self)
		self.mutex=mutex

	def run(self):
		global buff
		global cap
		while(True):
			if(buff==cap):
				print("Buffer lleno!")
				sleep(2)
			if(buff<cap): 
                
				self.mutex.acquire()
				buff = buff+1
				print("Productor produce: ", buff)
				random=randrange(1,2)
				sleep(random)
				self.mutex.release()

class consumidor(Thread):
	def __init__(self, id, mutex):
		Thread.__init__(self)
		self.id=id
		self.mutex=mutex

	def run(self):
		global buff
		while(True):
			if(buff==0):
				print("Buffer vacio!")
				sleep(2)
			if(buff>0):
				self.mutex.acquire()
				buff = buff-1
				print("Consumidor ", self.id, " consume: ", buff)
				random=randrange(3,4)
				sleep(random)
				self.mutex.release()


print("Inicio del programa.")
mutex=Semaphore(1)
mutex2=Semaphore(2)
prod=productor(mutex)
cons1=consumidor(1, mutex2)
cons2=consumidor(2, mutex2)
prod.start()
cons1.start()
cons2.start()
prod.join()
cons1.join()
cons2.join()
