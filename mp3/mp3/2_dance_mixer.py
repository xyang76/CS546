from threading import Thread, Semaphore
from time import sleep
from itertools import cycle
from collections import deque
import random

#The number of leaders and followers & songs second
leaderNum, followerNum, songsSec = 2, 5, 5

#globle values/defs
rng = random.Random()
rng.seed(100)
wleader, wfollower = -1, -1
musiclock = Semaphore(0)
queuelock = Semaphore(0)
ldancefinish, fdancefinish = Semaphore(0), Semaphore(0)
leaderlock, followerlock = Semaphore(1), Semaphore(1)
leaders = deque()
followers= deque()

def bandThread():
    global songsSec
    for music in cycle(['waltz', 'tango', 'foxtrot']):
        musiclock.release()     #start music
        print("** Band leader started playing %s **" %(music))
        sleep(songsSec)         #play sonsSec(5) seconds.
        musiclock.acquire()     #end music
        while True:
           # print("len = %d and len=%d" %(len(leaders), len(followers)))
            queuelock.acquire()
            break
        print("** Band leader stopped playing %s **" %(music))

def dance(leader, follower):
    global wleader, wfollower
    print("Leader %d and Follower %d are dancing." %(leader, follower))
    wleader = -1
    wfollower = -1
    ldancefinish.release()
    fdancefinish.release()

def leaderThread():
    global wleader, wfollower, leaderNum, followerNum
    while True:
        leaderlock.acquire()
        #enter_floor
        if wfollower == -1:          #if no one waiting for partner
            musiclock.acquire()      #enter floor need music
            musiclock.release()
        leader = leaders.popleft()
        wleader = leader
        print("Leader %d entering floor." %leader)

        #dance
        if wfollower != -1:
            dance(leader, wfollower)
        while True:
            ldancefinish.acquire()
            break
        leaderlock.release()
        sleep(rng.random()*3)                    #dance time
        
        #line_up
        print("Leader %d getting back in line." %leader)
        leaders.append(leader);
        if len(leaders) == leaderNum and len(followers) == followerNum:
            queuelock.release()

def followerThread():
    global wleader, wfollower, leaderNum, followerNum
    while True:
        followerlock.acquire()
        #enter_floor
        if wleader == -1:            #if no one waiting for partner
            musiclock.acquire()      #enter floor need music
            musiclock.release()
        follower = followers.popleft()
        wfollower = follower
        print("Follower %d entering floor." %follower)

        #dance
        if wleader != -1:
            dance(wleader, follower)
        while True:
            fdancefinish.acquire()
            break
        followerlock.release()
        sleep(rng.random()*3)        #dance time
        
        #line_up
        print("Follower %d getting back in line." %follower)
        followers.append(follower);
        if len(leaders) == leaderNum and len(followers) == followerNum:
            queuelock.release()
    
      
def main():
    global leaderNum, followerNum
    
    for i in range(leaderNum):
        leaders.append(i)
    for i in range(followerNum):
        followers.append(i)
    tl = [Thread(target=leaderThread) for i in range(leaderNum)]
    tf = [Thread(target=followerThread) for i in range(followerNum)]
    band = Thread(target=bandThread)
    band.start()
    for t in tl: t.start()
    for t in tf: t.start()
    band.join();
    for t in tl: t.join()
    for t in tf: t.join()

if __name__ == '__main__':
    main()
