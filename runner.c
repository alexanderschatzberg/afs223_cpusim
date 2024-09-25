#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sol.h"

hashtable *data;
uint64_t registers[32] = {0};

int main(int argc, char *argv[])
{
  data = ht_init();

  for (int i = 1; i < argc; ++i)
  {
    char *split = strchr(argv[i], '@');

    int index = atoi(argv[i]);
    uint64_t value = strtoull(split + 1, NULL, 16);

    if (index >= 0 && index <= 31)
    {
      registers[index] = value;
    }
  }

  uint32_t *instructions = malloc(200 * sizeof(uint32_t));
  int num_instructions = 0;
  while (fread(&instructions[num_instructions], sizeof(instructions[num_instructions]), 1, stdin))
  {
    num_instructions++;
  }

  uint64_t PC = 0;
  while (PC / 4 < (uint64_t)num_instructions)
  {
    uint32_t instruction = fetch(PC, instructions);
    info information;
    information = decode(instruction);
    information = execute(information);
    information = memory(information);
    PC = writeback(PC, information);
  }
  for (int i = 0; i < 32; ++i)
  {
    printf("Register[%d] = 0x%" PRIx64 "\n", i, registers[i]);
  }

  ht_free(data);
  free(instructions);

  return 0;
}
