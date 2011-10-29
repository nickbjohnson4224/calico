import sys

EMPTY = 0
BLACK = 1
WHITE = -1

class Move:

    def __init__(self, x, y):

        self.x = x
        self.y = y

class Position:

    def __init__(self, color = EMPTY):

        self.color = color
        self.group = None
        self.libs = 0
        self.rank = 0

    def get_libs(self):
        return self.get_group().libs

    def set_libs(self, libs = 0):
        self.get_group().libs = libs

    def add_libs(self, libs = 0):
        self.get_group().libs += libs

    def set_group(self, group = None):
        self.group = group

    def get_group(self):

        if self.group:
            self.group = self.group.get_group()
            return self.group
        else:
            return self

    def merge_group(self, group):

        group1 = self.get_group()
        group2 = group.get_group()

        if group1 != group2:
            libs = group1.libs + group2.libs;

            if group1.rank < group2.rank:
                group1.group = group2
                group2.libs += group1.libs
            elif group2.rank < group1.rank:
                group2.group = group1
                group1.libs += group2.libs
            else:
                group1.group = group2
                group2.libs += group1.libs
                group2.rank += 1

class Board:
    
    def __init__(self, dim = 19):

        self.board = []
        self.xdim = dim
        self.ydim = dim
        self.last = None
        self.llast = None
        self.player = BLACK
        self.ko = None

        for i in range(0, dim):
            self.board += [[]]
            for j in range(0, dim):
                self.board[i] += [ Position() ]

    def get(self, pos):

        if not pos:
            return None
        
        if pos.x < 1 or pos.y < 1 or pos.x > self.xdim or pos.y > self.ydim:
            return None
        
        return self.board[pos.x - 1][pos.y - 1]

    def validate_pos(self, pos):

        if not pos:
            return None

        if pos.x < 1 or pos.y < 1 or pos.x > self.xdim or pos.y > self.ydim:
            return None

        return pos

    def get_adj_pos(self, pos, direction):
        
        if not pos:
            return None

        if direction == 0:
            return self.validate_pos(Move(pos.x + 1, pos.y))
        if direction == 1:
            return self.validate_pos(Move(pos.x, pos.y + 1))
        if direction == 2:
            return self.validate_pos(Move(pos.x - 1, pos.y))
        if direction == 3:
            return self.validate_pos(Move(pos.x, pos.y - 1))

    def get_adj_list(self, pos):
        
        adj = []

        for i in range(0, 4):
            if self.get_adj_pos(pos, i):
                adj += [ self.get_adj_pos(pos, i) ]

        return adj

    def capture(self, pos):

        if not self.get(pos):
            return

        color = self.get(pos).color
        self.get(pos).color = EMPTY
        self.get(pos).group = None
        self.get(pos).libs  = 0
        self.get(pos).rank  = 0

        for i in self.get_adj_list(pos):
            color1 = self.get(i).color
            if color1 == -color:
                self.get(i).add_libs(1)
            elif color1 == color:
                self.capture(i)
    
    def place(self, pos, player):
        
        if not pos or not self.get(pos):
            return

        self.ko = None
        adj = []

        # get adjacent groups
        adj = self.get_adj_list(pos)

        # reduce liberties of all adjacent groups
        for i in adj:
            self.get(i).add_libs(-1)
        
        libs = 0
        for i in adj:
            color = self.get(i).color

            # capture all adjacent enemy groups with no liberties
            if color == -player:
                if self.get(i).get_libs() <= 0:
                    self.capture(i)
                    self.ko = pos
                    libs += 1

            # count liberties of added piece
            elif color == Go.EMPTY:
                libs += 1
        
        self.get(pos).libs  = libs
        self.get(pos).color = player
        self.get(pos).group = None
        self.get(pos).rank  = 0

        # merge with adjacent allied groups
        for i in adj:
            if self.get(i).color == player:
                self.get(pos).merge_group(self.get(i))
                self.ko = None

        self.llast = self.last
        self.last = pos

    def check(self, pos, player):
        
        # make sure space is open
        if self.get(pos).color != Go.EMPTY:
            return False

        # make sure there are no ko captures
        if self.ko and self.get(self.ko).get_libs() == 1:
            for i in self.get_adj_list(pos):
                if i == self.ko:
                    return False

        # make sure there is no suicide
        for i in self.get_adj_list(pos):
            
            libs = 0

            if self.get(i).color == EMPTY:
                return True

            for j in self.get_adj_list(pos):
                if self.get(j).group == self.get(i).group:
                    libs += 1
            
            if self.get(i).color == player:
                if libs != self.get(i).get_libs():
                    return True
            elif self.get(i).color == -player:
                if libs >= self.get(i).get_libs():
                    return True
        
        return False

    def try_place(self, pos, player):
        
        if self.check(pos, player):
            self.place(pos, player)

    def score(self):
        
        b = 0
        w = 0
        for x in range(1, self.xdim + 1):
            for y in range(1, self.ydim + 1):
                pos = Move(x, y)

                if self.get(pos).color == WHITE:
                    w += 1
                elif self.get(pos).color == BLACK:
                    b += 1
                else:
                    for j in self.get_adj_list(pos):
                        if self.get(j).color == BLACK:
                            b += 1
                        elif self.get(j).color == WHITE:
                            w += 1

        return [ b - w, b, w ]
