//
// Created by citrate on 11/23/21.
//

#ifndef DICE_H
#define DICE_H

#define DICE_DEVS 3  /* Number of dice devices */
#define DICE_NUM 2   /* Initial number of dice in a device */
#define GEN_SIDES 20 /* default number of sides of `arbitrary` dice game */

#define ERROR -1
#define MAX_STR 1024

// Dice Types
#define TYPE_RGLR 0 // Regular Dice
#define TYPE_BGMN 1 // Backgammon Die
#define TYPE_GENC 2 // Generic Dice

struct dice_dev{
    int num;               /* number of dice in this device */
    int dice_type;	       /* type of dice game:  regular; backgammon; generic */
    struct cdev dice_cdev; /* cdev object of dice */
};

#endif //DICE_H
