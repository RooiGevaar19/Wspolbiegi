#!/bin/bash
xterm -e ./scrabble.app & xterm -e ./scrabble.app || echo "The program cannot run. Probably the program \"xterm\" is not installed on this system."