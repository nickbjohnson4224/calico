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

class MoveGenerator:
    
    def __init__(self, board):
        self.board = board

    def generate(self):
        
        pos = None

        for i in range(1, 100):
            x = int(random.random() * self.board.xdim)
            y = int(random.random() * self.board.ydim)
            pos = go.Position(x, y)

            if is_bad_move(self.board, pos, self.board.player):
                pos = None
                continue

            break

        return pos

def is_bad_move(board, pos, player):
    
    try: board.check(pos, board.player)
    except go.IllegalMoveError:
        return True

    adj = 0
    for i in board.get_adj_list(pos):
        if board.get(i).color == player:
            adj += 1
        if board.get(i).get_libs() == 1:
            return False

    if adj == 4:
        return True;

    return False
