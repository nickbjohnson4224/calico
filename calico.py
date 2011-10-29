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
import go

class DistributionGenerator:
    
    def __init__(self, board):
        
        self.xdim = board.xdim
        self.ydim = board.ydim

    def weight(self, pos):

        return 1.0

    def max_weight(self):

        return 1.0

class LightDistributionGenerator(DistributionGenerator):
    
    def __init__(self, board):
       
        self.board = board
        self.xdim = board.xdim
        self.ydim = board.ydim

    def is_sane(self, pos):
    
        try: self.board.check(pos, self.board.player)
        except go.IllegalMoveError:
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

    def weight(self, pos):
        
        if self.is_sane(pos):
            return 1.0
        return 0.0

    def max_weight(self):

        return 1.0
      
class CachedDistributionGenerator(DistributionGenerator):
    
    def __init__(self, dg):
        
        self.xdim = dg.xdim
        self.ydim = dg.ydim

        # build cache
        self.mcache = dg.max_weight()
        self.ncache = dg.weight(None)
        self.wcache = []
        for x in range(0, dg.xdim):
            self.wcache += [[]]
            for y in range(0, dg.ydim):
                self.wcache[x] += [ dg.weight(go.Position(x + 1, y + 1)) ]

    def weight(self, pos):
        
        if pos:
            try:
                weight = self.wcache[pos.x - 1][pos.y - 1]
                return weight
            except IndexError:
                return 0.0
        else:
            return self.ncache

    def max_weight(self):

        return self.mcache

    def flush(self):

        # rebuild cache
        self.mcache = dg.max_weight()
        self.ncache = dg.weight(None)
        for x in range(0, dg.xdim):
            for y in range(0, dg.ydim):
                self.wcache[x][y] = dg_weight(go.Position(x + 1, y + 1));

class MoveGenerator:

    def __init__(self, board):
        self.board = board

    def is_sane(self, pos):
    
        try: self.board.check(pos, self.board.player)
        except go.IllegalMoveError:
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
        return None 

class RandomDistributionMoveGenerator(MoveGenerator):
    
    def __init__(self, dg):
        self.dg = dg;
    
    def generate(self):
        
        while True:

            # propose a move
            x = int(random.random() * self.dg.xdim) + 1
            y = int(random.random() * self.dg.ydim) + 1
            pos = go.Position(x, y)
            if (random.random() < 1.0 / (self.dg.xdim * self.dg.ydim)):
                pos = None

            # test that move
            weight = self.dg.weight(pos)
            if weight == 0.0:
                continue
            if random.random() < weight / self.dg.max_weight():
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
                pos = go.Position(x, y)

                if maximum < self.dg.weight(pos):
                    maximum = self.dg.weight(pos)
                    maxmove = pos

        return maxmove
