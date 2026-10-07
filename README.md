# 15-Puzzle Solver Using Branch and Bound

 Project Overview

This project is a **15-Puzzle Solver implemented in C** using the **Branch and Bound algorithm**.

The 15-Puzzle consists of **15 numbered tiles and one blank space** arranged in a 4 × 4 grid. The objective is to move the tiles and reach the predefined goal arrangement.

The program checks whether the given puzzle is solvable and, if it is, finds a sequence of moves to reach the goal state.

---

 Objective

The main objectives of this project are:

* To implement the **15-Puzzle problem** using C.
* To solve the puzzle using the **Branch and Bound technique**.
* To use **Manhattan Distance** as the heuristic function.
* To check whether the given puzzle configuration is solvable.
* To display the sequence of moves required to reach the goal state.
* To optionally display all intermediate board configurations.

---

 Problem Statement

Given a 4 × 4 puzzle containing numbers **1 to 15 and one blank space (0)**, find a sequence of valid tile movements that transforms the given initial arrangement into the predefined goal arrangement.

### Initial Arrangement Example

```text
1   2   3   4
5   6   7   8
9  10   0  12
13 14  11  15
```

### Goal Arrangement

```text
1   2   3   4
5   6   7   8
9  10  11  12
13 14  15   0
```

Here, **0 represents the blank space**.

---

 Technologies Used

* **Programming Language:** C
* **Algorithm:** Branch and Bound
* **Heuristic:** Manhattan Distance
* **Data Structures:**

  * Min Heap
  * Hash Table
  * Linked Parent Nodes
  * Arrays

---

 How the Algorithm Works

The solver uses **Branch and Bound** to explore possible puzzle states.

For every generated state, a bound is calculated:

```text
Bound = Level + Manhattan Distance
```

Where:

* **Level** = Number of moves made from the initial state.
* **Manhattan Distance** = Estimated number of tile movements required to reach the goal.

The state with the smallest bound is given priority using a **Min Heap**.

A **Hash Table** is also used to store visited states and avoid unnecessary repeated exploration.

---

 Manhattan Distance

Manhattan Distance measures how far each tile is from its correct position.

For each tile:

```text
Manhattan Distance =
|Current Row - Goal Row| +
|Current Column - Goal Column|
```

The distances of all tiles are added together.

The blank tile (`0`) is not included in the calculation.

---

 Solvability Check

Before starting the search, the program checks whether the given puzzle can be solved.

The check is based on:

* Number of inversions
* Position of the blank tile
* Parity of the initial and goal configurations

If the puzzle is not solvable, the program displays:

```text
Result: UNSOLVABLE
```

Otherwise, the program starts searching for the solution.

---

 Program Features

The program provides a menu with the following options:

```text
1. Enter initial arrangement
2. Toggle show-every-step display
3. Check solvability
4. Solve puzzle
5. Show current boards
6. Exit
```

### 1. Enter Initial Arrangement

The user enters **16 numbers from 0 to 15**.

Example:

```text
1 2 3 4 5 6 7 8 9 10 0 12 13 14 11 15
```

The program validates that every number from 0 to 15 occurs exactly once.

---

### 2. Show Every Step

This option allows the user to turn the intermediate-board display **ON or OFF**.

When enabled, the program displays every board configuration from the initial state to the goal state.

---

### 3. Check Solvability

The program determines whether the entered puzzle can be solved.

Example:

```text
Status: SOLVABLE
```

or

```text
Status: UNSOLVABLE
```

---

### 4. Solve Puzzle

The program:

1. Displays the initial board.
2. Displays the fixed goal board.
3. Checks solvability.
4. Calculates the Manhattan Distance.
5. Performs Branch and Bound search.
6. Finds the solution path.
7. Displays the number of expanded states.
8. Displays the solution cost.
9. Displays the sequence of moves.

Example:

```text
Solvable/unsolvable status : SOLVABLE
Number of expanded states : ...
Solution cost (moves) : ...
Move sequence : Down, Right, ...
```

---

 Data Structures Used

### 1. Node

Each puzzle state is stored as a `Node`.

It contains:

* Board configuration
* Blank position
* Current level
* Bound value
* Move used to reach the state
* Parent node

The parent pointer is used to reconstruct the final solution path.

---

### 2. Min Heap

The Min Heap stores generated puzzle states.

The node having the **smallest bound** is removed first.

This helps the Branch and Bound algorithm explore promising states before less promising states.

---

### 3. Hash Table

The Hash Table stores previously visited puzzle configurations.

It helps to:

* Detect duplicate states.
* Avoid unnecessary exploration.
* Store the best level at which a state was reached.

---

 Possible Moves

The blank space can move in four directions:

```text
U → Up
D → Down
L → Left
R → Right
```

The program automatically determines which moves are legal based on the blank tile's position.

It also avoids immediately reversing the previous move.

---

How to Run

### Step 1: Save the Code

Save the program as:

```text
15_puzzle.c
```

### Step 2: Compile

Using GCC:

```bash
gcc 15_puzzle.c -o 15_puzzle
```

### Step 3: Run

```bash
./15_puzzle
```

On Windows:

```bash
15_puzzle.exe
```

---

 Example Input

When the program asks for the initial arrangement, enter:

```text
1 2 3 4 5 6 7 8 9 10 0 12 13 14 11 15
```

The corresponding board is:

```text
1   2   3   4
5   6   7   8
9  10   0  12
13 14  11  15
```

---

 Goal State

The program uses a fixed goal state:

```text
1   2   3   4
5   6   7   8
9  10  11  12
13 14  15   0
```

The user does not need to enter the goal arrangement.

---
 Team Contributions

| Team Member  | Contribution                                                                     |
| ------------ | -------------------------------------------------------------------------------- |
| Om Pednekar | Developed the Branch and Bound algorithm and core solving logic.                 |
| Om Gavali | Implemented input handling, board validation, and solvability checking.          |
| Jagruti Dhangar | Implemented Manhattan Distance, Min Heap, and Hash Table.                        |
| Rehan Momin | Implemented solution-path reconstruction, board display, testing, and debugging. |


Project Structure

```text
15-Puzzle-Solver-Branch-and-Bound/
│
├── 15_puzzle.c
└── README.md
```

---

Key Features

* ✅ 4 × 4 15-Puzzle
* ✅ Branch and Bound algorithm
* ✅ Manhattan Distance heuristic
* ✅ Solvability checking
* ✅ Min Heap for state selection
* ✅ Hash Table for visited states
* ✅ Solution path reconstruction
* ✅ Move sequence display
* ✅ Intermediate board display
* ✅ Input validation
* ✅ Fixed standard goal state

---

 Conclusion

The project demonstrates how the **Branch and Bound algorithm** can be applied to solve the 15-Puzzle problem efficiently.

By combining **Manhattan Distance, Min Heap, and Hash Table**, the program prioritizes promising puzzle states while avoiding unnecessary repeated states. It also provides the complete sequence of moves required to reach the goal configuration.
