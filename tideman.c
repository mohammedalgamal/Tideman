#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool makes_circle(int cycle_start, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int diff;
    for (int i = 0; i < pair_count; i++)
    {
        int min_diff = 0;
        int idx = -1;
        pair temp;
        for (int j = i + 1; j < pair_count; j++)
        {
            diff = (preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner]) -
                   (preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner]);
            if (diff < min_diff)
            {
                min_diff = diff;
                idx = j;
            }
        }
        if (idx != -1)
        {
            temp = pairs[i];
            pairs[i] = pairs[idx];
            pairs[idx] = temp;
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!makes_circle(pairs[i].winner, pairs[i].loser))
        {
            // Lock the pair unless it makes a circle
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Recursive function to check if entry makes a circle
bool makes_circle(int cycle_start, int loser)
{
    if (loser == cycle_start)
    {
        // If the current loser is the cycle start
        // The entry makes a circle
        return true;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i])
        {
            if (makes_circle(cycle_start, i))
            {
                // Forward progress through the circle
                return true;
            }
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    /*int nlo[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
    //int win[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
    int mw = 0;
    int idx = 0;
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count; j++)
        {
            if (locked[j][i] == true || i == j)
            {
                break;
            }
            nlo[idx] = j;
            idx++;
            //printf("%s\n", candidates[j]);
        }
    }
    for (int k = 0; k < candidate_count; k++)
    {
        int nw = 0;
        if (nlo[k] != -1)
        {
            for (int l = 0; l < candidate_count; l++)
            {
                if(locked[nlo[k]][l] == true)
                {
                    nw++;
                }
            }
            if (nw > mw)
            {
                mw = nw;
            }
        }
    }
    for (int m = 0; m < candidate_count; m++)
    {
        int nnw = 0;
        if (nlo[m] != -1)
        {
            for (int n = 0; n < candidate_count; n++)
            {
                if(locked[nlo[m]][n] == true)
                {
                    nnw++;
                }
            }
        }
        if (nnw == mw)
        {
            printf("%s\n", candidates[m]);
        }
    }
    return;*/

    for (int i = 0; i < candidate_count; i++)
    {
        bool isLoser = false;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                isLoser = true;
                break;
            }

        }

        if (isLoser)
        {
            continue;
        }
        if (!isLoser)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}