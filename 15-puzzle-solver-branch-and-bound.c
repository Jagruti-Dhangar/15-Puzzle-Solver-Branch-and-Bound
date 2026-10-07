#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define N 4
#define TOTAL (N * N)
#define MAX_EXPAND 200000
#define TABLE_SIZE (1 << 21)

/* ---------- Node ---------- */

typedef struct Node {
    uint8_t board[TOTAL];
    int blank;
    int level;
    int bound;
    char move;
    struct Node *parent;
} Node;

/* ---------- Min Heap ---------- */

typedef struct {
    Node **arr;
    int size;
    int capacity;
} Heap;

static void heap_init(Heap *h)
{
    h->capacity = 1024;
    h->size = 0;
    h->arr = (Node **)malloc(sizeof(Node *) * h->capacity);
}

static void heap_push(Heap *h, Node *node)
{
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->arr = (Node **)realloc(
            h->arr,
            sizeof(Node *) * h->capacity
        );
    }

    int i = h->size++;
    h->arr[i] = node;

    while (i > 0) {
        int parent = (i - 1) / 2;

        if (h->arr[parent]->bound <= h->arr[i]->bound)
            break;

        Node *tmp = h->arr[parent];
        h->arr[parent] = h->arr[i];
        h->arr[i] = tmp;

        i = parent;
    }
}

static Node *heap_pop(Heap *h)
{
    if (h->size == 0)
        return NULL;

    Node *top = h->arr[0];

    h->arr[0] = h->arr[--h->size];

    int i = 0;

    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < h->size &&
            h->arr[left]->bound < h->arr[smallest]->bound)
            smallest = left;

        if (right < h->size &&
            h->arr[right]->bound < h->arr[smallest]->bound)
            smallest = right;

        if (smallest == i)
            break;

        Node *tmp = h->arr[smallest];
        h->arr[smallest] = h->arr[i];
        h->arr[i] = tmp;

        i = smallest;
    }

    return top;
}

/* ---------- Visited State Hash Table ---------- */

typedef struct {
    uint64_t key;
    int level;
    bool used;
} HTEntry;

static HTEntry *g_table;

static uint64_t pack_board(const uint8_t board[])
{
    uint64_t packed = 0;

    for (int i = 0; i < TOTAL; i++) {
        packed |= ((uint64_t)board[i]) << (4 * i);
    }

    return packed;
}

static uint64_t hash_key(uint64_t key)
{
    key *= 11400714819323198485ULL;
    return key >> (64 - 21);
}

static int table_get(uint64_t key)
{
    uint64_t idx = hash_key(key);

    while (g_table[idx].used) {
        if (g_table[idx].key == key)
            return g_table[idx].level;

        idx = (idx + 1) & (TABLE_SIZE - 1);
    }

    return -1;
}

static void table_set(uint64_t key, int level)
{
    uint64_t idx = hash_key(key);

    while (g_table[idx].used &&
           g_table[idx].key != key)
        idx = (idx + 1) & (TABLE_SIZE - 1);

    g_table[idx].used = true;
    g_table[idx].key = key;
    g_table[idx].level = level;
}

/* ---------- Validation ---------- */

static bool validate_tiles(
    const uint8_t board[],
    char *err,
    size_t errlen)
{
    bool seen[TOTAL] = { false };

    for (int i = 0; i < TOTAL; i++) {

        if (board[i] > 15 || seen[board[i]]) {
            snprintf(
                err,
                errlen,
                "Tiles must be exactly 0-15, each occurring once."
            );

            return false;
        }

        seen[board[i]] = true;
    }

    return true;
}

/* ---------- Solvability ---------- */

static int count_inversions(const uint8_t board[])
{
    uint8_t no_blank[TOTAL - 1];
    int k = 0;

    for (int i = 0; i < TOTAL; i++) {
        if (board[i] != 0)
            no_blank[k++] = board[i];
    }

    int inv = 0;

    for (int i = 0; i < k; i++) {
        for (int j = i + 1; j < k; j++) {

            if (no_blank[i] > no_blank[j])
                inv++;
        }
    }

    return inv;
}

static int blank_index(const uint8_t board[])
{
    for (int i = 0; i < TOTAL; i++) {

        if (board[i] == 0)
            return i;
    }

    return -1;
}

static bool is_solvable(
    const uint8_t start[],
    const uint8_t goal[])
{
    int s_inv = count_inversions(start);
    int g_inv = count_inversions(goal);

    int s_row = N - (blank_index(start) / N);
    int g_row = N - (blank_index(goal) / N);

    int start_parity = (s_inv + s_row) % 2;
    int goal_parity = (g_inv + g_row) % 2;

    return start_parity == goal_parity;
}

/* ---------- Manhattan Distance ---------- */

static int manhattan_distance(
    const uint8_t board[],
    const int goal_pos[16][2])
{
    int total = 0;

    for (int i = 0; i < TOTAL; i++) {

        int tile = board[i];

        if (tile == 0)
            continue;

        int r = i / N;
        int c = i % N;

        int gr = goal_pos[tile][0];
        int gc = goal_pos[tile][1];

        total += abs(gr - r) + abs(gc - c);
    }

    return total;
}

/* ---------- Legal Moves ---------- */

static int legal_moves(int blank, char moves[4])
{
    int r = blank / N;
    int c = blank % N;
    int n = 0;

    if (r > 0)
        moves[n++] = 'U';

    if (r < N - 1)
        moves[n++] = 'D';

    if (c > 0)
        moves[n++] = 'L';

    if (c < N - 1)
        moves[n++] = 'R';

    return n;
}

static int move_offset(char move)
{
    switch (move) {

        case 'U':
            return -N;

        case 'D':
            return N;

        case 'L':
            return -1;

        case 'R':
            return 1;
    }

    return 0;
}

static char opposite(char move)
{
    switch (move) {

        case 'U':
            return 'D';

        case 'D':
            return 'U';

        case 'L':
            return 'R';

        case 'R':
            return 'L';
    }

    return 0;
}

static void apply_move(
    const uint8_t board[],
    int blank,
    char move,
    uint8_t out[],
    int *new_blank)
{
    memcpy(out, board, TOTAL);

    int nb = blank + move_offset(move);

    out[blank] = out[nb];
    out[nb] = 0;

    *new_blank = nb;
}

/* ---------- Branch and Bound ---------- */

static Node *solve(
    const uint8_t start[],
    const uint8_t goal[],
    const int goal_pos[16][2],
    int *expansions)
{
    memset(
        g_table,
        0,
        sizeof(HTEntry) * TABLE_SIZE
    );

    *expansions = 0;

    Node *root = (Node *)malloc(sizeof(Node));

    memcpy(root->board, start, TOTAL);

    root->blank = blank_index(start);
    root->level = 0;
    root->parent = NULL;
    root->move = 0;

    /* Bound = Level + Manhattan Distance */

    root->bound =
        manhattan_distance(start, goal_pos);

    Heap open;

    heap_init(&open);
    heap_push(&open, root);

    table_set(pack_board(start), 0);

    while (open.size > 0) {

        Node *node = heap_pop(&open);

        /* Goal test */

        if (memcmp(node->board, goal, TOTAL) == 0) {
            free(open.arr);
            return node;
        }

        int best =
            table_get(pack_board(node->board));

        if (best < node->level)
            continue;

        (*expansions)++;

        if (*expansions > MAX_EXPAND) {
            free(open.arr);
            return NULL;
        }

        char moves[4];

        int nmoves =
            legal_moves(node->blank, moves);

        for (int i = 0; i < nmoves; i++) {

            char m = moves[i];

            /* Avoid immediate reversal */

            if (node->move != 0 &&
                m == opposite(node->move))
                continue;

            uint8_t new_board[TOTAL];
            int new_blank;

            apply_move(
                node->board,
                node->blank,
                m,
                new_board,
                &new_blank
            );

            int new_level =
                node->level + 1;

            uint64_t key =
                pack_board(new_board);

            int existing =
                table_get(key);

            if (existing == -1 ||
                new_level < existing) {

                table_set(key, new_level);

                Node *child =
                    (Node *)malloc(sizeof(Node));

                memcpy(
                    child->board,
                    new_board,
                    TOTAL
                );

                child->blank = new_blank;
                child->level = new_level;
                child->parent = node;
                child->move = m;

                child->bound =
                    new_level +
                    manhattan_distance(
                        new_board,
                        goal_pos
                    );

                heap_push(&open, child);
            }
        }
    }

    free(open.arr);

    return NULL;
}

/* ---------- Solution Path ---------- */

static int reconstruct_path(
    Node *goal_node,
    Node *path[],
    char moves[])
{
    int count = 0;

    Node *n = goal_node;
    Node *stack[10000];

    while (n != NULL) {
        stack[count++] = n;
        n = n->parent;
    }

    for (int i = 0; i < count; i++) {
        path[i] =
            stack[count - 1 - i];
    }

    int nmoves = 0;

    for (int i = 1; i < count; i++) {
        moves[nmoves++] =
            path[i]->move;
    }

    return nmoves;
}

/* ---------- Display ---------- */

static void print_board(const uint8_t board[])
{
    for (int r = 0; r < N; r++) {

        for (int c = 0; c < N; c++) {

            int v = board[r * N + c];

            if (v == 0)
                printf(" ");
            else
                printf("%2d ", v);
        }

        printf("\n");
    }

    printf("\n");
}

static void read_arrangement(uint8_t board[])
{
    printf(
        "\nEnter 16 numbers (0-15, 0 = blank):\n"
    );

    for (int i = 0; i < TOTAL; i++) {

        int v;

        while (scanf("%d", &v) != 1) {

            printf(
                "Please enter integers only.\n"
            );

            int c;

            while ((c = getchar()) != '\n' &&
                   c != EOF) {
            }
        }

        board[i] = (uint8_t)v;
    }
}

static void move_name(char m, char *out)
{
    switch (m) {

        case 'U':
            strcpy(out, "Up");
            break;

        case 'D':
            strcpy(out, "Down");
            break;

        case 'L':
            strcpy(out, "Left");
            break;

        case 'R':
            strcpy(out, "Right");
            break;

        default:
            strcpy(out, "?");
            break;
    }
}

static void build_goal_pos(
    const uint8_t goal[],
    int goal_pos[16][2])
{
    for (int i = 0; i < TOTAL; i++) {

        int tile = goal[i];

        goal_pos[tile][0] = i / N;
        goal_pos[tile][1] = i % N;
    }
}

/* ---------- MAIN ---------- */

int main(void)
{
    /* Fixed goal - no option to enter another goal */

    static const uint8_t GOAL[TOTAL] =
    {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 0
    };

    g_table =
        (HTEntry *)malloc(
            sizeof(HTEntry) * TABLE_SIZE
        );

    uint8_t start[TOTAL];

    bool have_start = false;
    bool show_steps = false;

    Node *last_path[10000];
    char last_moves[10000];

    int last_nmoves = -1;
    int choice;

    while (1) {

        printf(
            "\n============================================================\n"
        );

        printf(
            " 15-PUZZLE SOLVER - BRANCH AND BOUND\n"
        );

        printf(
            "============================================================\n"
        );

        printf(
            "1. Enter initial arrangement%s\n",
            have_start ? " [set]" : ""
        );

        printf(
            "2. Toggle show-every-step display [%s]\n",
            show_steps ? "ON" : "OFF"
        );

        printf(
            "3. Check solvability\n"
        );

        printf(
            "4. Solve puzzle\n"
        );

        printf(
            "5. Show current boards\n"
        );

        printf(
            "6. Exit\n"
        );

        printf(
            "Choose an option (1-6): "
        );

        if (scanf("%d", &choice) != 1) {

            int c;

            while ((c = getchar()) != '\n' &&
                   c != EOF) {
            }

            printf("Invalid option.\n");
            continue;
        }

        char err[128];

        switch (choice) {

            /* Enter puzzle */

            case 1:
            {
                uint8_t tmp[TOTAL];

                read_arrangement(tmp);

                if (!validate_tiles(
                        tmp,
                        err,
                        sizeof(err))) {

                    printf(
                        "Invalid input: %s\n",
                        err
                    );

                } else {

                    memcpy(
                        start,
                        tmp,
                        TOTAL
                    );

                    have_start = true;

                    printf(
                        "Initial arrangement saved.\n"
                    );
                }

                break;
            }

            /* Toggle step display */

            case 2:

                show_steps = !show_steps;

                printf(
                    "Show-every-step display is now %s.\n",
                    show_steps ? "ON" : "OFF"
                );

                break;

            /* Solvability */

            case 3:

                if (!have_start) {

                    printf(
                        "Enter initial arrangement first.\n"
                    );

                    break;
                }

                printf(
                    "Status: %s\n",
                    is_solvable(start, GOAL)
                        ? "SOLVABLE"
                        : "UNSOLVABLE"
                );

                break;

            /* Solve */

            case 4:
            {
                if (!have_start) {

                    printf(
                        "Enter initial arrangement first.\n"
                    );

                    break;
                }

                printf(
                    "\nInitial board:\n"
                );

                print_board(start);

                printf(
                    "Fixed goal board:\n"
                );

                print_board(GOAL);

                if (!is_solvable(start, GOAL)) {

                    printf(
                        "Result: UNSOLVABLE\n"
                    );

                    break;
                }

                int goal_pos[16][2];

                build_goal_pos(
                    GOAL,
                    goal_pos
                );

                printf(
                    "Result: SOLVABLE - searching...\n\n"
                );

                int expansions = 0;

                Node *result =
                    solve(
                        start,
                        GOAL,
                        goal_pos,
                        &expansions
                    );

                if (result == NULL) {

                    printf(
                        "No solution found within search limit.\n"
                    );

                    break;
                }

                last_nmoves =
                    reconstruct_path(
                        result,
                        last_path,
                        last_moves
                    );

                printf(
                    "------------------------------------------------------------\n"
                );

                printf(
                    "Solvable/unsolvable status : SOLVABLE\n"
                );

                printf(
                    "Number of expanded states : %d\n",
                    expansions
                );

                printf(
                    "Solution cost (moves) : %d\n",
                    last_nmoves
                );

                printf(
                    "Move sequence : "
                );

                if (last_nmoves == 0) {

                    printf(
                        "(already at goal)\n"
                    );

                } else {

                    for (int i = 0;
                         i < last_nmoves;
                         i++) {

                        char name[8];

                        move_name(
                            last_moves[i],
                            name
                        );

                        printf(
                            "%s%s",
                            name,
                            (i == last_nmoves - 1)
                                ? "\n"
                                : ", "
                        );
                    }
                }

                printf(
                    "------------------------------------------------------------\n"
                );

                if (show_steps) {

                    printf(
                        "\nIntermediate boards:\n\n"
                    );

                    for (int i = 0;
                         i <= last_nmoves;
                         i++) {

                        if (i == 0) {

                            printf("Start\n");

                        } else {

                            char name[8];

                            move_name(
                                last_moves[i - 1],
                                name
                            );

                            printf(
                                "Move %d: %s\n",
                                i,
                                name
                            );
                        }

                        print_board(
                            last_path[i]->board
                        );
                    }
                }

                break;
            }

            /* Show boards */

            case 5:

                if (!have_start) {

                    printf(
                        "Nothing entered yet.\n"
                    );

                    break;
                }

                printf(
                    "\nInitial board:\n"
                );

                print_board(start);

                printf(
                    "Fixed goal board:\n"
                );

                print_board(GOAL);

                if (last_nmoves >= 0)

                    printf(
                        "Last solution: %d moves.\n",
                        last_nmoves
                    );

                break;

            /* Exit */

            case 6:

                printf(
                    "Exiting. Goodbye!\n"
                );

                free(g_table);

                return 0;

            default:

                printf(
                    "Invalid option. Choose 1-6.\n"
                );
        }
    }

    return 0;
}
