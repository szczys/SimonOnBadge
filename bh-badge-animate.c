#include "HaD_Badge.h"

uint8_t ballX = 4;
uint8_t ballY = 9;

//Hint: look in HaD_Badge.h for function and constant definitions

void eraseBall() {
    //uses global variables to erase current ball
    displayPixel(ballX, ballY, OFF);
    displayLatch();
}

void moveLeft() {
    if (ballX > 0) {
        //only move if we're not already at the edge
        eraseBall();
        --ballX;
        displayPixel(ballX, ballY, ON);
        displayLatch();
    }
}

void moveRight() {
    if (ballX < TOTPIXELX-1) {
        //only move if we're not already at the edge
        eraseBall();
        ++ballX;
        displayPixel(ballX, ballY, ON);
        displayLatch();
    }
}

void moveUp() {
    if (ballY > 0) {
        //only move if we're not already at the edge
        eraseBall();
        --ballY;
        displayPixel(ballX, ballY, ON);
        displayLatch();
    }
}

void moveDown() {
    if (ballY < TOTPIXELY-1) {
        //only move if we're not already at the edge
        eraseBall();
        ++ballY;
        displayPixel(ballX, ballY, ON);
        displayLatch();
    }
}

uint8_t pattern[32] = { 0,0,1,0,2,2,0,3,1,3,1,3,2,0,0,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//Initialize pattern tracking vars
//TODO: should be a function
uint8_t patternIdx = 0;
uint8_t lastPatternIdx = 255;
uint8_t scoreAndProgress = 0;
uint32_t patternDelay = 250;
uint32_t storedDelay = 250;
uint8_t gameState = 0;

void generatePattern(uint16_t seed) {
    //TODO: use seed with prng. Currently we're using hardcoded pattern
    
    //do nothing because we're faking pattern generation for now
    
}

void drawPatternInstance(uint8_t patternVal) {
    displayClear();
    //This turns on a block of LEDs representing Top, Right, Down, or Left (0..3)
    switch(patternVal) {
        case (0):
            //UP
            for (uint8_t i=0; i<8; i++) {
                Buffer[i] = 0xFF;
            }
            break;
        case (1):
            //RIGHT
            for (uint8_t i=0; i<16; i++) {
                Buffer[i] = 0x0F;
            }
            break;
        case (2):
            //DOWN
            for (uint8_t i=8; i<16; i++) {
                Buffer[i] = 0xFF;
            }
            break;
        case (3):
            //LEFT
            for (uint8_t i=0; i<16; i++) {
                Buffer[i] = 0xF0;
            }
            break;
    }
    displayLatch();
}

void userMimic(uint8_t instance) {
    //Light up the instance
    drawPatternInstance(instance);
    controlDelayMs(patternDelay);   //FIXME: Dirty blocking delay
    displayClear();
    displayLatch();
    
    //Check if it's correct
    if (pattern[patternIdx] == instance) {
        //yes correct
        ++patternIdx;
        //are we done?
        if (patternIdx > scoreAndProgress) {
            //if patternIdx overflow
            //yes, signal user, reset state
            controlDelayMs(1000);   //FIXME: Show a confirmation here and maybe don't block
            ++scoreAndProgress;
            //change state
            gameState = 0;
            //reset patternIdx
            patternIdx = 0;
            lastPatternIdx = 255;
            return;
        }
        //no, keep going
            
    }
    else {
        //incorrect: game over
        for (uint8_t i=6; i<10; i++) {
            Buffer[i] = 0x3C;
        }
        Buffer[15] = scoreAndProgress;  //Show score in little endian binary
        displayLatch();
        //FIXME: Need to do more game over stuff here
        gameState = 0;
        patternIdx = 0;
        lastPatternIdx = 255;
        scoreAndProgress = 0;
        controlDelayMs(4000);
    } 
}

void playSimon(void) {
    //Generate pattern
    generatePattern(0);
    
    uint32_t nextTime = getTime()+storedDelay;

    
    while (1) {
        //TODO: There should be a way to break this loop, right?
        
        if (getTime() > nextTime) {
            nextTime = getTime()+storedDelay;
            if (gameState == 0) {
                //Playback pattern
                //need some function to draw blocks on display

                if (patternIdx != lastPatternIdx) {
                    //Turn on current pattern
                    drawPatternInstance(pattern[patternIdx]);
                    //Set wait timeout
                    storedDelay = patternDelay;
                    //Rest lastPatternIdx
                    lastPatternIdx = patternIdx;
                }
                else {
                    //Turn off current pattern
                    displayClear();
                    displayLatch();
                    //increment pattern
                    ++patternIdx;
                    
                    //Set wait timeout
                    storedDelay = patternDelay;
                    
                    if (patternIdx > scoreAndProgress) {
                        //if patternIdx overflow
                            //change state
                            gameState = 1;
                            //reset patternIdx
                            patternIdx = 0;
                            lastPatternIdx = 255;
                    }
                }

                //TODO: we should speed up (decrease patternDelay) as the pattern gets longer
            }
            else {
                //Pause and poll for user input
                //was it correct? play next step in pattern or game over
            }
        }
        
        //This shows how to get user input
        switch (getControl()) {
            case (ESCAPE):
                displayClose();
                return;
            case (LEFT):
                if (gameState != 0) {
                    userMimic(3);
                }
                break;
            case (RIGHT):
                if (gameState != 0) {
                    userMimic(1);
                }
                break;
            case (UP):
                if (gameState != 0) {
                    userMimic(0);
                }
                break;
            case (DOWN):
                if (gameState != 0) {
                    userMimic(2);
                }
                break;
            
        }
    }
}

void animateBadge(void) {
    displayPixel(ballX, ballY, ON);
    displayLatch();
    uint32_t nextTime = getTime();

    while(1) {
        
        //This shows how to use non-blocking getTime() function
        
        if (getTime() > nextTime) {
            //lastTime = getTime();
            nextTime = getTime()+1000;
            Buffer[14] ^= 0xFF;
            displayLatch();
        }
        
        
        //Buffer[14] ^= 0xFF;
        
        //This shows how to get user input
        
        switch (getControl()) {
            case (ESCAPE):
                displayClose();
                return;
            case (LEFT):
                moveLeft();
                break;
            case (RIGHT):
                moveRight();
                break;
            case (UP):
                moveUp();
                break;
            case (DOWN):
                
                playSimon();
                return;
                
                moveDown();
                break;
        }
        
    }
}