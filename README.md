# P2 - One Card Game

## About this game
```
 _____   __  __  ____        ____     ______  ____    ____      
/\  __`\/\ \/\ \/\  _`\     /\  _`\  /\  _  \/\  _`\ /\  _`\    
\ \ \/\ \ \ `\\ \ \ \L\_\   \ \ \/\_\\ \ \L\ \ \ \L\ \ \ \/\ \  
 \ \ \ \ \ \ , ` \ \  _\L    \ \ \/_/_\ \  __ \ \ ,  /\ \ \ \ \ 
  \ \ \_\ \ \ \`\ \ \ \L\ \   \ \ \L\ \\ \ \/\ \ \ \\ \\ \ \_\ \
   \ \_____\ \_\ \_\ \____/    \ \____/ \ \_\ \_\ \_\ \_\ \____/
    \/_____/\/_/\/_/\/___/      \/___/   \/_/\/_/\/_/\/ /\/___/                                    
```


_One Card Game_ is quite similar to _UNO_. It uses Poker cards (without jokers) instead of UNO cards, 
and 2 stands for '+2' card, 3 stands for '+3', of course. Queen for 'reverse' and Jack for 'skip'.

For detailed game rule please click gamerule.txt here.

## Overview
_note that the cards are stackable, not display bug_

_and the cards are actually colored_
```
Onecard                                           p2->p4->p1                                            score:0
last card:  addings:0                                                                     pool:2515,discard:4
.------.
|8.--. |
| :/\: |
| (__) |
| '--'8|
`------'
your cards :20
.------..------..------..------..------..------..------..------..------..------..------..------..------.
|Q.--. ||K.--. ||2.--. ||5.--. ||9.--. ||5.--. ||6.--. ||9.--. ||9.--. ||10--. ||10--. ||Q.--. ||K.--. |
.------..------..------..------..------..------..------.| :/\: || :/\: || :/\: || :/\: || :/\: || :/\: |
|2.--. ||2.--. ||6.--. ||7.--. ||9.--. ||K.--. ||A.--. || :\/: || :\/: || :\/: || :\/: || :\/: || :\/: |
| :(): || :(): || :(): || :(): || :(): || :(): || :(): || '--'9|| '--'9|| '--10|| '--10|| '--'Q|| '--'K|
| ()() || ()() || ()() || ()() || ()() || ()() || ()() |`------'`------'`------'`------'`------'`------'
| '--'2|| '--'2|| '--'6|| '--'7|| '--'9|| '--'K|| '--'A|
`------'`------'`------'`------'`------'`------'`------'

your choice :
1------.2------.
|Q.--. ||K.--. |
| :/\: || :/\: |
| (__) || (__) |
| '--'Q|| '--'K|
`------'`------'


give your choice(0 to draw card):
```

## Dependency

- cmake_minimum_required VERSION 3.5
- c_standard c11
- command window should be at least `92c * 20r` of size (bigger for better experience)
- command window should support ANSI escape code
- Linux , macOS , Windows all supported


## Quick start

- One computer mode: just run the program and give at least one argument to start

- Play over network: not supported yet, plan to support in the future

## Features

- [x] mutiplayer play on one computer(press enter and the window will be cleared so that the next player won't see the cards of the previous player)
- [x] really nice CLI interface, with color and animation(make sure your terminal supports ANSI escape code)
- [x] support various cmd window size, even if you change the size of the window during the game, the interface will be automatically adjusted
- [x] logfile for each game, you can check the game history in the log file

## Exit Code:
1: `calloc` or `realloc` failed

2: card pool error

3: player not found
