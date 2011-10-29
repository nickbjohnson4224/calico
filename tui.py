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
import sys
import string

exit = "exit"

def printf(fmt, *args):
    sys.stdout.write(fmt % args)

def display(board):
    letters = " ABCDEFGHJKLMNOPQRST"

    printf("\n    ")
    for x in range(1, board.xdim + 1):
        printf("%c ", letters[x])
    printf("\n")

    for y in range(board.ydim, 0, -1):
        printf(" %2d", y)

        if board.last == go.Position(1, y):
            printf("[")
        else:
            printf(" ")

        for x in range(1, board.xdim + 1):
                
            libs = board.get(go.Position(x, y)).get_libs()

            if board.get(go.Position(x, y)).color == go.WHITE:
                printf("O")
            elif board.get(go.Position(x, y)).color == go.BLACK:
                printf("#")
            else:
                if (x - 3) % 6 == 1 and (y - 3) % 6 == 1 and (board.xdim == 19 or board.xdim == 13):
                    printf("+")
                else:
                    printf("-")
                
            if board.last == go.Position(x, y):
                printf("]")
            elif board.last == go.Position(x + 1, y):
                printf("[")
            else:
                printf(" ")

        printf("%-2d\n", y)

    printf("    ")
    for x in range(1, board.xdim + 1):
        printf("%c ", letters[x])
    printf("\n\n")

class ExitException(Exception):

    def __init__(self):
        pass

def read_move():
    
    value = raw_input("enter a move: ")

    if value == "pass":
        return None

    if value == "exit":
        raise ExitException()

    letter = ord(string.upper(value[0])) - ord('A') + 1
    number = int(value[1:])
    if letter > 8: letter -= 1

    if letter < 1 or letter > 25 or number < 1:
        raise ValueError()

    return go.Position(letter, number)
