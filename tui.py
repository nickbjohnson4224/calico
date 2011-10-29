import go
import sys

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

        if board.last == go.Move(1, y):
            printf("[")
        else:
            printf(" ")

        for x in range(1, board.xdim + 1):
                
            libs = board.get(go.Move(x, y)).get_libs()

            if board.get(go.Move(x, y)).color == go.WHITE:
                printf("O")
            elif board.get(go.Move(x, y)).color == go.BLACK:
                printf("#")
            else:
                if (x - 3) % 6 == 1 and (y - 3) % 6 == 1 and (board.xdim == 19 or board.xdim == 13):
                    printf("+")
                else:
                    printf("-")
                
            if board.last == go.Move(x, y):
                printf("]")
            elif board.last == go.Move(x + 1, y):
                printf("[")
            else:
                printf(" ")

        printf("%-2d\n", y)

    printf("    ")
    for x in range(1, board.xdim + 1):
        printf("%c ", letters[x])
    printf("\n\n")
