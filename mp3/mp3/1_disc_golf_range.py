from threading import Thread, Semaphore
from time import sleep
import random

#stash size, perbucket and forlfer numbers
stashSize, perbucket, frolfers = 20, 5, 3

#globle values/defs
rng = random.Random()
rng.seed(100)
stash, discs_on_field = 0, 0
cartstart = Semaphore(0)
frolferlock = Semaphore(1)
cartfinish = Semaphore(0)
printlock = Semaphore(1)        #This lock keep the print value consistent.

def frolferThread(n):
    global stash, perbucket, discs_on_field
    bucket = 0;
    while True:
        frolferlock.acquire()   #When cart, all frolferlock should lock
        frolferlock.release()
        if bucket == 0:
            printlock.acquire()
            print("Frolfer %d calling for bucket" %n)
            printlock.release()
            if(stash < perbucket):
                cartstart.release()
                cartfinish.acquire()
            stash -= perbucket  #stash-perbucket should before bucket=perbucket in multiple thread
            bucket = perbucket
            printlock.acquire()
            print("Frolfer %d got %d discs; Stash=%d" %(n, perbucket, stash))
            printlock.release()
        printlock.acquire()
        print("Frolfer %d threw disc %d" %(n, perbucket-bucket))
        printlock.release()
        bucket-=1
        discs_on_field +=1
        sleep(rng.random()*3)
    
def cartThread():
    global stash, discs_on_field
    while True:
        cartstart.acquire()
        frolferlock.acquire()       #lock all frolfer thread
        print("################################################################################")
        print("Stash=%d, Cart entering field" %stash)
        stash += discs_on_field
        print("Cart done, gathered %d discs; Stash = %d" %(discs_on_field, stash))
        discs_on_field = 0
        print("################################################################################")
        frolferlock.release()
        cartfinish.release()
      
def main():
    global stash, perbucket, frolfers, stash
    stash = stashSize
    ts = [Thread(target=frolferThread, args=[i]) for i in range(frolfers)]
    cart = Thread(target=cartThread)
    for t in ts: t.start()
    cart.start()
    for t in ts: t.join()
    cart.join();

if __name__ == '__main__':
    main()
