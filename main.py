#! /usr/bin/env python

# Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
# 
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

import go
import tui
import sys
import copy
import calico

if len(sys.argv) < 2:
    board = go.Board()
else :
    dim = int(sys.argv[1])
    board = go.Board(dim, dim)

tui.display(board)

g = calico.UCTMoveGenerator(board)
move = (1000, 1000)

try:
    while True:

        # user's turn
        try:
            move = tui.read_move()
        except tui.ExitException:
            sys.exit(0)
        except ValueError as v:
            print "Illegal input"
            continue

        if not move: passes += 1
        else: passes = 0

        if passes == 2: break
  
        try:
            board.place(move)
        except go.IllegalMoveError as i:
            print "Illegal move:", i
            continue
        
        tui.display(board)

        # computer's turn
        
        g = calico.UCTMoveGenerator(board)
        for i in range(g.plays, 100): g.playout()

        g.display()

        move = g.generate_conservative()

        print move

        if not move: passes += 1
        else: passes = 0

        if passes == 2: break

        board = g.child[move].board
        tui.display(board)

        # computer's turn
#        if move in g.child:
#            g = g.child[move]
#        else:
#            g = calico.UCTMoveGenerator(board)
#        for i in range(g.plays, 100): g.playout()
#
#        g.display()
#
#        move = g.generate_conservative()
#
 #       print move
#
 #       if not move: passes += 1
  #      else: passes = 0
#
 #       if passes == 2: break
#
 #       board = g.child[move].board
  #      tui.display(board)
    
    score = board.score()[0] + .5
    if score < 0:
        print "final score: W+%.1f" % -score
    else:
        print "final score: B+%.1f" % score

except KeyboardInterrupt:
    print ""
    sys.exit(0)
