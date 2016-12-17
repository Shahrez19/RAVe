import pygame;
import serial;
import os;
from pygame.locals import *;
    
def clear_screen():
    os.system('Clear Screen')

def getCommand(run):
    for event in pygame.event.get():
        print "Anything";
        if (event.type == KEYDOWN):
            keyinput = pygame.key.get_pressed();

            if keyinput[pygame.K_UP]:
                print "up button pressed"
                run[1] = 1;

            elif keyinput[pygame.K_x] or keyinput[pygame.K_q]:
                print "Invalid button pressed"
                print 'exit'
                run[1] = 0;

        elif event.type == pygame.KEYUP:
            if (run[1] < 20):
                run[1] = 0;
 #   run = [False,1]
    return run;
    
def main():

    clear_screen()
    
    print '\n STARTING MAIN \n';

    ## ACM0 ends with a zero
    ## Always change this to the corresponding arduino PORT
    ser = serial.Serial('/dev/ttyACM0', 115200,timeout=1)

    pygame.init();
    run = [True,0]
    previous = -1

    while run[0]:
        run = getCommand(run);
        print run

        # debug
        #print commands

        if(run[1] != previous):
            previous = run[1];
            ser.write(chr(run[1] + 65));
            print run[1];
    
    ser.close();

    exit('\nDONE!\n')

if __name__ == "__main__":
    main()
