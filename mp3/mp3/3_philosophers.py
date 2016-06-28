from threading import Thread, Semaphore
from time import sleep
import time
import random
import sys

#The number of philosophers and meals
#You also can define them in console such as
#   $ python 3_philosophers.py 20 1000
philosophers, meals = 20, 1000

#globle values/defs
rng = random.Random()
rng.seed(100)
forks = None
mealsRemain = 0

class Footman:
    def __init__(this):
        this.footman = Semaphore(philosophers-1)
    def left(this, i):
        return i
    def right(this, i):
        global philosophers
        return (i+1) % philosophers
    def get_forks(this, i):
        global forks
        this.footman.acquire()
        forks[this.right(i)].acquire()
        forks[this.left(i)].acquire()
    def put_forks(this, i):
        global forks
        forks[this.right(i)].release()
        forks[this.left(i)].release()
        this.footman.release()
        
class LeftHanded:
    def left(this, i):
        return i
    def right(this, i):
        global philosophers
        return (i+1) % philosophers
    def get_forks(this, i):
        global forks
        if i==0:
            forks[this.left(i)].acquire()
            forks[this.right(i)].acquire()
        else:
            forks[this.right(i)].acquire()
            forks[this.left(i)].acquire()
    def put_forks(this, i):
        global forks
        if i==0:
            forks[this.left(i)].release()
            forks[this.right(i)].release()
        else:
            forks[this.right(i)].release()
            forks[this.left(i)].release()
class Tanenbaum:
    def __init__(this):
        global philosophers
        this.state = ["thinking"]*philosophers
        this.sem = [Semaphore(0) for i in range(philosophers)]
        this.mutex = Semaphore(1)
    def left(this, i):
        global philosophers
        return (i+philosophers-1) % philosophers
    def right(this, i):
        global philosophers
        return (i+1) % philosophers
    def get_forks(this, i):
        this.mutex.acquire()
        this.state[i] = "hungry"
        this.test(i)
        this.mutex.release()
        this.sem[i].acquire()
    def put_forks(this, i):
        this.mutex.acquire()
        this.state[i] = "thinking"
        this.test(this.right(i))
        this.test(this.left(i))
        this.mutex.release()
    def test(this, i):
        if this.state[i] == "hungry" and this.state[this.left(i)] != "eating" and this.state[this.right(i)] != "eating":
            this.state[i] = "eating"
            this.sem[i].release()
        
def thread(tar, i):
    global mealsRemain
    while(mealsRemain>0):
        #eat
        #print("I am running, meal is %d" %mealsRemain)
        mealsRemain -=1
        tar.get_forks(i)
        sleep(rng.random()/50)
        #think
        tar.put_forks(i)
        sleep(rng.random()/50)

def getTime(tar):
    global philosophers, mealsRemain, meals
    start = time.time()
    mealsRemain = meals
    ts = [Thread(target=thread, args=[tar, i]) for i in range(philosophers)]
    for t in ts: t.start()
    for t in ts: t.join()
    return time.time()-start;
        
def main():
    global philosophers, meals, forks
    if len(sys.argv) >= 3:
        philosophers = int(sys.argv[1])
        meals = int(sys.argv[2])
    print("Running dining philosophers simulation: %d philosophers, %d meals each"%(philosophers,meals))
    forks = [Semaphore(1) for i in range(philosophers)]
    print("1. Footman solution, time elapsed:{:0.3f}s".format(getTime(Footman())))
    print("2. Left-handed solution, time elapsed:{:0.3f}s".format(getTime(LeftHanded())))
    print("3. Tanenbaum's solution, time elapsed:{:0.3f}s".format(getTime(Tanenbaum())))
    
    
if __name__ == '__main__':
    main()
