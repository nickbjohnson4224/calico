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

#board.place(go.Position(1, 1))
#tui.display(board)

passes = 0

#generator_factory = lambda b: calico.RandomDistributionMoveGenerator(calico.LightDistributionGenerator(b))
generator_factory = lambda b: calico.MoveGenerator(b)

#import profile
#profile.run('for i in range(0, 10): calico.playout(board, generator_factory)')

for i in range(0, 100):
    print i
    board1 = calico.playout(board, generator_factory)

tui.display(board1)
print "score: ", board.score()

sys.exit(0)

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
        d = calico.LightDistributionGenerator(board)
        g = calico.RandomDistributionMoveGenerator(d)
        move = g.generate()

        if not move: passes += 1
        else: passes = 0

        if passes == 2: break

        board.place(move)
        tui.display(board)
    
    score = board.score()[0] + .5
    if score < 0:
        print "final score: W+%.1f" % -score
    else:
        print "final score: B+%.1f" % score

except KeyboardInterrupt:
    print ""
    sys.exit(0)
