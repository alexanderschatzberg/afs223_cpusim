#define ADD 0
#define SUB 1
#define AND 2
#define SLT 3
#define SLL 4
#define SRA 5
#define ADDI 6
#define ANDI 7
#define LD 8
#define LW 9
#define LB 10
#define SD 11
#define SW 12
#define SB 13
#define LUI 14
#define BEQ 15
#include "hash_table.h"

/*
 * This hash table is used to simulate memory.
 * It will map addresses to data in memory.
 * We suggest mapping an address to one byte of data,
 * but an alternative such as mapping addresses to
 * four or eight bytes is also acceptable.
 */
extern hashtable *data;

extern uint64_t registers[32];

/*
 * The "info" struct holds the information that stages
 * pass between each other, with the exception of fetch
 * pasing the instruction to decode. info is
 * intentionally just a container for arbitrary bits and
 * there is no single correct way to map from its
 * fields to the relevant state. Additionally, the info
 * struct will be used in different ways for each of
 * the stage -> stage communication steps, and there
 * is no relationship between them.
 */
typedef struct
{
  uint32_t inst;
  uint64_t first;
  uint64_t second;
  uint64_t third;
} info;

/*
 * Returns the current instruction according to the
 * program counter (PC).
 */
uint32_t fetch(uint64_t PC, uint32_t *instructions);

/*
 * Returns the results of the decode stage.
 * The info struct returned by decode is passed as
 * the input to execute.
 */
info decode(uint32_t instruction);

/*
 * Returns the results of the execute stage.
 * The info struct returned by execute is passed as
 * the input to memory.
 */
info execute(info information);

/*
 * Updates the memory hash table appropriately.
 * Returns the results of the memory stage.
 * The info struct returned by memory is passed as
 * the input to writeback.
 */
info memory(info information);

/*
 * Updates the registers appropriately.
 * Returns the program counter (PC) of the next
 * instruction.
 */
uint64_t writeback(uint64_t PC, info information);
