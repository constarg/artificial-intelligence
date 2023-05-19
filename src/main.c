#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct region 
{
    uint8_t r_num;                 // The number of the region
    uint8_t r_n_num;               // The neighbour number
    char *r_color;                 // The color of the region. blue, red, green, yellow
    struct region *r_neighbour[6]; // The neightbour regions
};

// All the regions.
static struct region g_candidates[4][17]; 
static int g_chosen_probability[4] = {0}; // The probability to be chosen for next gen
static int g_candidates_scores[4] = {0}; // The score of each candidate.
static int g_probability_gen_sum = 0; // curr generation probability sum. 

static char *g_colors[4] = {
    // b        r        g       y
    "000\0", "001\0", "010\0", "011\0"
};

/**
 * Candidate is the current sample of the evolution.
 * @param candidate_regions The current candidate parent/child 
*/
static inline int fitness_function(const struct region *candidate_regions)
{
    int same_color = 0; // count how many neightbours have the same color.
    struct region curr_region;
    struct region curr_neightbour;

    // ignore the 0, start from one to asociate the same numbers as the grapth
    // because it was more convenient when I did the configuration (configure_regions, func)
    for (int i = 1; i < 17; i++) {
        // loop throuth each region and check it's neightbours.
        curr_region = candidate_regions[i];
        // loop each neightbour
        for (int j = 0; j < curr_region.r_n_num; j++) {
            curr_neightbour = *curr_region.r_neighbour[j];
            // if the current region and an neightbour has the same color
            if (!strcmp(curr_region.r_color, curr_neightbour.r_color)) {
                ++same_color;
            }
        }
    }

    return same_color;
}

/**
 * @param src The region to configure
 * @param n_num The number of neightbours
 * @param neightbours // The neightbours of src.
*/
static void configure_region(struct region *src, uint8_t n_num,
                             const struct region *neightbours, ...)
{
    // allocate space the the neightbours.
    src->r_n_num = n_num;
    src->r_neighbour[0] = (struct region *) neightbours;
    va_list n_list;
    va_start(n_list, neightbours);
    for (int i = 1; i < n_num; i++) {
        // Set the neightbours.
        src->r_neighbour[i] = (struct region *) va_arg(n_list, void *);
    }
    va_end(n_list);
}

static inline void configure_regions(struct region *regions) 
{
    // start the array from because the numbers comes more naturaly
    configure_region(&regions[1],  6, &regions[2], &regions[3], &regions[4], &regions[13], &regions[15], &regions[16]);
    configure_region(&regions[2],  6, &regions[1], &regions[3], &regions[5], &regions[9], &regions[8], &regions[16]);
    configure_region(&regions[3],  5, &regions[1], &regions[2], &regions[4], &regions[5], &regions[6]);
    configure_region(&regions[4],  4, &regions[1], &regions[3], &regions[6], &regions[13]);
    configure_region(&regions[5],  6, &regions[2], &regions[3], &regions[6], &regions[7], &regions[9], &regions[10]);
    configure_region(&regions[6],  6, &regions[3], &regions[4], &regions[5], &regions[7], &regions[11], &regions[13]);
    configure_region(&regions[7],  4, &regions[5], &regions[6], &regions[10], &regions[11]);
    configure_region(&regions[8],  3, &regions[2], &regions[9], &regions[14]);
    configure_region(&regions[9],  6, &regions[2], &regions[5], &regions[8], &regions[10], &regions[12], &regions[14]);
    configure_region(&regions[10], 5, &regions[5], &regions[7], &regions[9], &regions[11], &regions[12]);
    configure_region(&regions[11], 5, &regions[6], &regions[7], &regions[10], &regions[12], &regions[13]);
    configure_region(&regions[12], 6, &regions[9], &regions[10], &regions[11], &regions[13], &regions[14], &regions[15]);
    configure_region(&regions[13], 5, &regions[6], &regions[11], &regions[12], &regions[15], &regions[1]);
    configure_region(&regions[14], 5, &regions[2], &regions[8], &regions[9], &regions[12], &regions[15]);
    configure_region(&regions[15], 6, &regions[1], &regions[2], &regions[12], &regions[13], &regions[14], &regions[16]);
    configure_region(&regions[16], 3, &regions[1], &regions[2], &regions[15]);
}

static void print_grapth(const struct region *candidate_regions)
{
    // TODO - print the grapth
}

static void sort_scores()
{
    int tmp;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3 - i; j++) {
            if (g_candidates_scores[j] > g_candidates_scores[j + 1]) {
                tmp = g_candidates_scores[j + 1];
                g_candidates_scores[j + 1] = g_candidates_scores[j];
                g_candidates_scores[j] = tmp;
            }
        }
    }
}

static int inline decide_candidate(int decide)
{
    int tmp_score = g_candidates_scores[0];
    int tmp_decide = decide;
    int curr_candidate = 0;
    while (tmp_decide > tmp_score) {
        tmp_decide -= tmp_score;
        ++curr_candidate;
        tmp_score = g_candidates_scores[curr_candidate];
    }
    return curr_candidate;
}

// Check if the new partners are the same as the previous two, but backwards.
static int is_same_partners(int chosen_p1, int chosen_p2,
                            int old_chosen_p1, int old_chosen_p2) 
{
    if (chosen_p1 == old_chosen_p1 && chosen_p2 == old_chosen_p2) {
        return 1;
    } else if (chosen_p1 == old_chosen_p2 && chosen_p2 == old_chosen_p1) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    int tmp_score = 0;
    // The 4 candidates choose for the next generation.
    struct region *set_of_partners[4];
    int whitch_partner_1 = 0;
    int whitch_partner_2 = 0;
    int old_whitch_partners_1 = 0;
    int old_whitch_partners_2 = 0;

    srand(time(0));
    // configure the regions.
    for (int i = 0; i < 4; i++) {
        for (int j = 1; j < 17; j++) {
            // Assign the number of the region.
            g_candidates[i][j].r_num = j;
            // Assign an initial random color.
            g_candidates[i][j].r_color = g_colors[rand() % 4];
            configure_regions(g_candidates[i]);
        }
    }

//    while (1) {
        // meseaure the score of each candidate
        for (int i = 0; i < 4; i++) {
            tmp_score = fitness_function(g_candidates[i]);\
            g_candidates_scores[i] = tmp_score;
            g_probability_gen_sum += tmp_score;
        }

        // measure the probabilty of each candidate
        for (int i = 0; i < 4; i++) {
            g_chosen_probability[i] = g_probability_gen_sum / g_candidates_scores[i];
        }   
        // Sort the scores.
        sort_scores();
        // select partners
        for (int i = 0; i <= 2; i += 2) {
    redo:   whitch_partner_1 = rand() % (g_probability_gen_sum + 1);
            whitch_partner_2 = rand() % (g_probability_gen_sum + 1);
            whitch_partner_1 = decide_candidate(whitch_partner_1);
            whitch_partner_2 = decide_candidate(whitch_partner_2);
            // Check if the partners are the same, or are again the same but backwards.
            if (whitch_partner_1 == whitch_partner_2 ||
                is_same_partners(whitch_partner_1, whitch_partner_2, old_whitch_partners_1, old_whitch_partners_2)) goto redo;

            set_of_partners[i]     = g_candidates[whitch_partner_1];
            set_of_partners[i + 1] = g_candidates[whitch_partner_2];

            old_whitch_partners_1 = whitch_partner_1;
            old_whitch_partners_2 = whitch_partner_2;
        }
        old_whitch_partners_1 =  0;
        old_whitch_partners_2 = 0;
//    }
    // TODO - validate the set_of_partners
    // TODO - make it loop until you found a solution.
    return 0;
}
