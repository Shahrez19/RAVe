import pygame;
import serial;
import os;
from pygame.locals import *;
    
def clear_screen():
    os.system('Clear Screen')

def getCommand(run):
    for event in pygame.event.get():
        if(event.type == KEYDOWN):
            keyinput = pygame.key.get_pressed();

            #Simple commands :: one key Pressed

            if keyinput[pygame.K_UP]:
                run[1] = 11;
            elif keyinput[pygame.K_DOWN]:
                run[1] = 12;
            elif keyinput[pygame.K_RIGHT]:
                run[1] = 13;
            elif keyinput[pygame.K_LEFT]:
                run[1] = 14;

            elif keyinput[pygame.K_x] or keyinput[pygame.K_q]:
                print 'exit'
                run[0] = 'False';
                run[1] = 0;

        elif event.type == pygame.KEYUP:
            if(run[1] < 20):
                run[1] = 0;
        return run;
    
def main():
    clear_screen()

    print '\n Starting ControlCar \n';

    ## ACM0 ends with a zero
    ser = serial.Serial('/dev/ttyACM0', 115200,timeout=1)

    pygame.init();
    run = [True,0]
    previous = -1

    while run[0]:
        run = getCommand(run);

        # debug
        #print commands

        if(run[1] != previous):
            previous = run[1];
            ser.write(chr(run[1]) + 65);
            print run[1];
    
    ser.close();

    exit('\nDONE!\n')

if __name__ == "__main__":
    main()
