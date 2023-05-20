#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>

#define COLOR_BLUE   "000\0"
#define COLOR_RED    "001\0"
#define COLOR_GREEN  "010\0"
#define COLOR_YELLOW "011\0"

#define REGION(reg) &regions[(reg) - 1]

struct region 
{
    uint8_t r_num;                 // The number of the region
    uint8_t r_n_num;               // The neighbour number
    char *r_color;                 // The color of the region. blue, red, green, yellow
    struct region *r_neighbour[6]; // The neightbour regions
};

// All the regions.
static struct region g_candidates[4][16]; 
static int g_candidates_scores[4] = {0}; // The score of each candidate.
static int g_scores_sum = 0; // curr generation probability sum. 

static char *g_colors[4] = {
    // b            r           g           y
    COLOR_BLUE, COLOR_RED, COLOR_GREEN, COLOR_YELLOW
};

/**
 * Candidate is the current sample of the evolution.
 * @param candidate_regions The current candidate parent/child 
*/
static inline int fitness_function(int *not_right, const struct region *candidate_regions)
{
    int diff_color = 0; // count how many neightbours have the same color.
    struct region curr_region;
    struct region curr_neightbour;

    for (int i = 0; i < 16; i++) {
        // loop throuth each region and check it's neightbours.
        curr_region = candidate_regions[i];
        // loop each neightbour
        for (int j = 0; j < curr_region.r_n_num; j++) {
            curr_neightbour = *curr_region.r_neighbour[j];
            // if the current region and an neightbour has the same color
            if (!strcmp(curr_region.r_color, curr_neightbour.r_color)) {
                *not_right = 1;
                continue;
            }
            ++diff_color;
        }
    }

    return diff_color;
}

/**
 * @param src The regicandidate_regions configure
 * @param n_num The number of neightbours
 * @param neightbours // The neightbours of src.
*/
static void configure_region(struct region *src, uint8_t n_num,
                             const struct region *neightbours, ...)
{
    src->r_n_num = n_num;
    src->r_neighbour[0] = (struct region *) neightbours;
    va_list n_list;
    va_start(n_list, neightbours);
    for (int i = 1; i < n_num; i++) {
        // Set the neightbours.
        src->r_neighbour[i] = (struct region *) va_arg(n_list, struct region *);
    }
    va_end(n_list);
}

static inline void configure_regions(struct region *regions) 
{
    // start the array from because the numbers comes more naturaly
    configure_region(REGION(1),  6, REGION(2), REGION(3), REGION(4), REGION(13), REGION(15), REGION(16));
    configure_region(REGION(2),  6, REGION(1), REGION(3), REGION(5), REGION(9), REGION(8), REGION(16));
    configure_region(REGION(3),  5, REGION(1), REGION(2), REGION(4), REGION(5), REGION(6));
    configure_region(REGION(4),  4, REGION(1), REGION(3), REGION(6), REGION(13));
    configure_region(REGION(5),  6, REGION(2), REGION(3), REGION(6), REGION(7), REGION(9), REGION(10));
    configure_region(REGION(6),  6, REGION(3), REGION(4), REGION(5), REGION(7), REGION(11), REGION(13));
    configure_region(REGION(7),  4, REGION(5), REGION(6), REGION(10), REGION(11));
    configure_region(REGION(8),  3, REGION(2), REGION(9), REGION(14));
    configure_region(REGION(9),  6, REGION(2), REGION(5), REGION(8), REGION(10), REGION(12), REGION(14));
    configure_region(REGION(10), 5, REGION(5), REGION(7), REGION(9), REGION(11), REGION(12));
    configure_region(REGION(11), 5, REGION(6), REGION(7), REGION(10), REGION(12), REGION(13));
    configure_region(REGION(12), 6, REGION(9), REGION(10), REGION(11), REGION(13), REGION(14), REGION(15));
    configure_region(REGION(13), 5, REGION(6), REGION(11), REGION(12), REGION(15), REGION(1));
    configure_region(REGION(14), 5, REGION(2), REGION(8), REGION(9), REGION(12), REGION(15));
    configure_region(REGION(15), 6, REGION(1), REGION(2), REGION(12), REGION(13), REGION(14), REGION(16));
    configure_region(REGION(16), 3, REGION(1), REGION(2), REGION(15));
}

// decode the current color code.
static inline char decode_color(const char *color_code)
{
    return (!strcmp(color_code, COLOR_BLUE))?   'B' : 
           (!strcmp(color_code, COLOR_RED))?    'R' :
           (!strcmp(color_code, COLOR_GREEN))?  'G' : 'Y';
}

static void print_grapth(const struct region *candidate_regions)
{
    char c_1, c_2, c_3, c_4, c_5, c_6, c_7, c_8;
    char c_9, c_10, c_11, c_12, c_13, c_14, c_15, c_16;
    c_1  = decode_color(candidate_regions[0].r_color);
    c_2  = decode_color(candidate_regions[1].r_color);
    c_3  = decode_color(candidate_regions[2].r_color);
    c_4  = decode_color(candidate_regions[3].r_color);
    c_5  = decode_color(candidate_regions[4].r_color);
    c_6  = decode_color(candidate_regions[5].r_color);
    c_7  = decode_color(candidate_regions[6].r_color);
    c_8  = decode_color(candidate_regions[7].r_color);
    c_9  = decode_color(candidate_regions[8].r_color);
    c_10 = decode_color(candidate_regions[9].r_color);
    c_11 = decode_color(candidate_regions[10].r_color);
    c_12 = decode_color(candidate_regions[11].r_color);
    c_13 = decode_color(candidate_regions[12].r_color);
    c_14 = decode_color(candidate_regions[13].r_color);
    c_15 = decode_color(candidate_regions[14].r_color);
    c_16 = decode_color(candidate_regions[15].r_color);

    printf("B: Blue, R: Red, G: Green, Y: Yellow\n");

    printf(" -------------------------------------------\n");
    printf("|                     %c                     |\n", c_1);
    printf("|---------------------------------------    |\n");
    printf("|                |      %c    |         |    |\n", c_3);
    printf("|           ----- -------------    %c   |    |\n", c_4);
    printf("|          |         |        |        |    |\n");
    printf("|    %c     |    %c  -----    %c |        |    |\n", c_2, c_5, c_6);
    printf("|          |      |   %c |     |--------     |\n", c_7);
    printf("|    --- ----- ------ ----- --     |        |\n");
    printf("|   |   |     |   %c  |  %c  |       |        |\n", c_10, c_11);
    printf("|   |   |   %c |      |     |   %c   |        |\n", c_9, c_13);
    printf("|   |   |      ------ -----        |        |\n");
    printf("|   |   |        |    %c    |       |        |\n", c_12);
    printf("|   |    ----- -- --------- -------         |\n");
    printf("|   |  %c      |   %c  |     %c    |           |\n", c_8, c_14, c_15);
    printf("|    --------- ------ ---------- -----------|\n");
    printf("|                            |     %c        |\n", c_16);
    printf(" -------------------------------------------\n");

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
    int tmp_decide = decide;

    int cnd; // current candidate.
    int tmp = 0;
    for (cnd = 0; cnd < 4; cnd++) {
        tmp = g_candidates_scores[cnd] - tmp_decide;;
        if (tmp < 0) continue;
        else if (tmp > 0) tmp_decide = tmp;
        else break;
    }
    if (cnd == 4) --cnd;

    return cnd;
}

/**
 * @param parent_1 The first parent
 * @param parent_2 The second parent.
 * @param first_parent_gen Decide where we use the parent_1 or parent_2 genetic first
*/
static struct region *create_offsprings(const struct region *parent_1,
                                        const struct region *parent_2) 
{
    struct region *offspring = (struct region *) malloc(sizeof(struct region) * 16);
    if (NULL == offspring) return NULL;
    memset(offspring, 0x0, sizeof(struct region));

    // transfer the cromosoms of the two parents in the child.
    for (int i = 0; i < 16; i++) {
        if (i < 8) {
            // half of the first parent
            offspring[i].r_color = parent_1[i].r_color;
        } else {
            // holf of the second parent.
            offspring[i].r_color = parent_2[i].r_color;
        }
    }

    return offspring;
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

/**
 * @param parent The parent to be replaced.
 * @param offspring The offspring that will replace the parent.
*/
static inline void replace_generation(struct region *parent, const struct region *offspring) 
{
    for (int i = 0; i < 16; i++) {
        parent[i].r_color = offspring[i].r_color;
    }
}

int main(int argc, char *argv[])
{
    int tmp_score = 0;
    // The 4 candidates choose for the next generation.
    int set_of_partners[4];
    int whitch_partner_1 = 0;
    int whitch_partner_2 = 0;
    int old_whitch_partners_1 = 0;
    int old_whitch_partners_2 = 0;
    int not_right = 0;
    // offsrpings.
    struct region *first_offspring;
    struct region *second_offspring;
    struct region *third_offspring;
    struct region *forth_offspring;

    srand(time(NULL));
    // configure the regions.
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 16; j++) {
            // Assign the number of the region.
            g_candidates[i][j].r_num = j;
            // Assign an initial random color.
            g_candidates[i][j].r_color = g_colors[rand() % 4];
        }
        configure_regions(g_candidates[i]);
    }
    while (1) {
        // meseaure the score of each candidate
        for (int i = 0; i < 4; i++) {
            tmp_score = fitness_function(&not_right, g_candidates[i]);
            // Check if any candidate has solved the puzzle
            if (!not_right) {
                printf("DONE!\n");
                printf("Candidate: %d\n", i + 1);
                print_grapth(g_candidates[i]);
                return 0;
            }
            g_candidates_scores[i] = tmp_score;
            g_scores_sum += tmp_score;
        }

        // select partners
        for (int i = 0; i <= 2; i += 2) {
    redo:   whitch_partner_1 = rand() % (g_scores_sum + 1); 
            whitch_partner_2 = rand() % (g_scores_sum + 1);
            whitch_partner_1 = decide_candidate(whitch_partner_1);
            whitch_partner_2 = decide_candidate(whitch_partner_2);
            // Check if the partners are the same, or are again the same but backwards.
            if (whitch_partner_1 == whitch_partner_2 ||
                is_same_partners(whitch_partner_1, whitch_partner_2, old_whitch_partners_1, old_whitch_partners_2)) goto redo;

            set_of_partners[i]     = whitch_partner_1;
            set_of_partners[i + 1] = whitch_partner_2;

            old_whitch_partners_1 = whitch_partner_1;
            old_whitch_partners_2 = whitch_partner_2;
        }
        old_whitch_partners_1 =  0;
        old_whitch_partners_2 = 0;
        // create offsrpings.
        first_offspring  = create_offsprings(g_candidates[set_of_partners[0]], g_candidates[set_of_partners[1]]);
        second_offspring = create_offsprings(g_candidates[set_of_partners[1]], g_candidates[set_of_partners[0]]);
        third_offspring  = create_offsprings(g_candidates[set_of_partners[2]], g_candidates[set_of_partners[3]]);
        forth_offspring  = create_offsprings(g_candidates[set_of_partners[3]], g_candidates[set_of_partners[2]]);
        if (NULL == first_offspring || NULL == second_offspring ||
            NULL == third_offspring || NULL == forth_offspring) return 0;

        // replace the old generation with the new.
        replace_generation(g_candidates[0], first_offspring);
        replace_generation(g_candidates[1], second_offspring);
        replace_generation(g_candidates[2], third_offspring);
        replace_generation(g_candidates[3], forth_offspring);
        // print the offsprings.
        for (int i = 0; i < 4; i++) print_grapth(g_candidates[i]);

        // free the space allocated for offsprings.
        free(first_offspring);
        free(second_offspring);
        free(third_offspring);
        free(forth_offspring);

    }

    // TODO - make it loop until you found a solution.
    return 0;
}
