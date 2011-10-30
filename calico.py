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

import random
import heapq
import copy
import math
import go

class DistributionGenerator:
    
    def __init__(self, board):
        
        self.xdim = board.xdim
        self.ydim = board.ydim

    def weight(self, pos):

        return 1.0

class LightDistributionGenerator(DistributionGenerator):
    
    def __init__(self, board):
       
        self.board = board
        self.xdim = board.xdim
        self.ydim = board.ydim

    def weight(self, pos):
   
        if not pos: return 0.5

        if not self.board.check_fast(pos, self.board.player):
            return 0.0

        adj = 0
        for i in self.board.get_adj_list(pos):
            if self.board.get(i).color == self.board.player:
                adj += 1
            if self.board.get(i).get_libs() == 1:
                return 1.0

        if adj == 4:
            return 0.0

        return 1.0

class MoveGenerator:

    def __init__(self, board):
        self.board = board

    def is_sane(self, pos):
    
        if not self.board.check_fast(pos, self.board.player):
            return False

        adj = 0
        for i in self.board.get_adj_list(pos):
            if self.board.get(i).color == self.board.player:
                adj += 1
            if self.board.get(i).get_libs() == 1:
                return True

        if adj == 4:
            return False

        return True

    def generate(self):
        
        for i in range(1, 20):

            x = int(random.random() * self.board.xdim) + 1
            y = int(random.random() * self.board.ydim) + 1
            pos = (x, y)

            if self.is_sane(pos):
                return pos

        return None
            

class RandomDistributionMoveGenerator(MoveGenerator):
    
    def __init__(self, dg):
        self.dg = dg;
    
    def generate(self):
        
        while True:

            # propose a move
            x = int(random.random() * self.dg.xdim) + 1
            y = int(random.random() * self.dg.ydim) + 1
            pos = (x, y)
            if (random.random() < 1.0 / (self.dg.xdim * self.dg.ydim)):
                pos = None

            # test that move
            weight = self.dg.weight(pos)
            if weight == 0.0:
                continue
            if random.random() < weight:
                return pos

class MaximumDistributionMoveGenerator(MoveGenerator):
    
    def __init__(self, dg):
        self.dg = dg;

    def generate(self):
       
        # maximize move weight over all moves
        maximum = self.dg.weight(None)
        maxmove = None
        for x in range(0, self.dg.xdim):
            for y in range(0, self.dg.ydim):
                pos = (x, y)

                if maximum < self.dg.weight(pos):
                    maximum = self.dg.weight(pos)
                    maxmove = pos

        return maxmove

class SortedDistributionMoveGenerator(MoveGenerator):

    def __init__(self, dg):
        
        self.heap = []
        for x in range(1, dg.xdim + 1):
            for y in range(1, dg.ydim + 1):
                pos = (x, y)
                heapq.heappush(self.heap, (-dg.weight(pos), pos))
        heapq.heappush(self.heap, (-dg.weight(None), None))

    def generate(self):
        
        tup = heapq.heappop(self.heap)
        pos = tup[1]

        heapq.heappush(self.heap, (tup[0] / 2.0, tup[1]))

        return pos

class RandomSortedDistributionMoveGenerator(MoveGenerator):
    
    def __init__(self, dg):
        
        self.heap = []
        for x in range(1, dg.xdim + 1):
            for y in range(1, dg.ydim + 1):
                pos = (x, y)
                heapq.heappush(self.heap, (-dg.weight(pos), random.random(), pos))
        heapq.heappush(self.heap, (-dg.weight(None), random.random(), None))

    def generate(self):
        
        tup = heapq.heappop(self.heap)
        pos = tup[2]

        heapq.heappush(self.heap, (tup[0] / 2.0, random.random(), tup[1]))

        if tup[0] == 0.0:
            return None

        return pos

def playout(board, generator_factory = None):
   
    board = copy.copy(board)

    if not generator_factory:
        generator_factory = lambda b: RandomDistributionMoveGenerator(LightDistributionGenerator(b))

    passes = 0
    while True:
        
        g = generator_factory(board)
        move = g.generate()

        if not move:
            passes += 1
        else:
            passes = 0

        board.place_unchecked(move, board.player)
        board.player = -board.player

        if passes == 2:
            return board

class UCTMoveGenerator(MoveGenerator):
    
    def __init__(self, board, parent = None):
        
        self.wins  = 1
        self.plays = 2

        self.board = board
        
        self.child  = {}
        self.parent = parent

    def __err(self, s, n):
        return (0.1 * math.log(s + 1) / math.log(self.board.xdim * self.board.ydim)) * math.sqrt(1.0 / n)

    def ucb(self):

        if self.parent:
            ucb = (float(self.wins) / self.plays) + self.__err(self.parent.plays, self.plays)
        else:
            ucb = (float(self.wins) / self.plays) + self.__err(self.plays, self.plays)

        return ucb

    def lcb(self):

        if self.parent:
            lcb = (float(self.wins) / self.plays) - self.__err(self.parent.plays, self.plays)
        else:
            lcb = (float(self.wins) / self.plays) - self.__err(self.plays, self.plays)

        return lcb

    def playout_norecurse(self):
        
        score = playout(self.board).score()
        if score[0] > .5:
            winner = go.BLACK
        else:
            winner = go.WHITE

        if winner == self.board.player:
            self.wins += 1
        self.plays += 1

        return winner

    def generate(self):

        heap = []

        try:
            child = self.child[None]
            heapq.heappush(heap, (child.lcb(), 1.0, None))
        except KeyError:
            heapq.heappush(heap, (0.0, 0.1, None))

        for x in range(1, self.board.xdim + 1):
            for y in range(1, self.board.ydim + 1):
                try:
                    child = self.child[(x, y)]
                    if child:
                        heapq.heappush(heap, (child.lcb(), random.random(), (x, y)))
                except KeyError:
                    heapq.heappush(heap, (0.0, random.random(), (x, y)))
        
        return heapq.heappop(heap)[2]

    def generate_conservative(self):
        
        heap = []
        heapq.heappush(heap, (0.9, 1.0, None))
        for x in self.child:
            child = self.child[x]
            if child:
                heapq.heappush(heap, (child.ucb(), random.random(), x))
        
        return heapq.heappop(heap)[2]

    def playout(self):
        
        # generate new valid move
        pos = self.generate()
        if pos in self.child:
            winner = self.child[pos].playout()
        else:
            while pos not in self.child:

                if not self.board.check_fast(pos, self.board.player):
                    self.child[pos] = None
                    pos = self.generate()
                    continue

                break
            
            board = copy.copy(self.board)
            board.place(pos)
            self.child[pos] = UCTMoveGenerator(board, self)
            winner = self.child[pos].playout_norecurse()
        
        if winner == self.board.player:
            self.wins += 1
        self.plays += 1

        return winner

    def display(self, level = 0):

        import sys
        
        def printf(fmt, *args):
            sys.stdout.write(fmt % args)
    
        for i in range(0, level): printf("\t")
        print self.board.last,
        printf(": [ %.2f, %.2f ] %d\n", self.lcb(), self.ucb(), self.plays)

        for c in self.child:
            if self.child[c]:
                self.child[c].display(level + 1)
