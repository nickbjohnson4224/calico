#! /usr/bin/env python

import go
import tui

board = go.Board(9)

tui.display(board)

print board.score()
