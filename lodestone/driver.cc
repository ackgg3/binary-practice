#include <stdio.h>
#include <stdint.h>
#include <string>

#include "lodestone.h"

int main(int argc, char* argv[])
{
  int i;
  Binary      bin;
  Section*    sec;
  Symbol*     sym;
  std::string fname;
  bool        dig = false;
  std::string digsection_name;
  Section*    digsection;
  uint64_t*     b_dig;

  if(argc < 2)
  {
    printf("Give me a binary!");
    return 1;
  }
  if(argc == 3)
  {
    dig = true;
    digsection_name.assign(argv[2]);
  }

  fname.assign(argv[1]);
  if(load_bin(fname, &bin, Binary::BIN_TYPE_AUTO) < 0)
    return -1;
  
  printf("loaded binary '%s', %s/%s (%u bits) entry @ 0x%016jx\n\n",
        bin.filename.c_str(),
        bin.type_str.c_str(),
        bin.arch_str.c_str(),
        bin.bits,
        bin.entry);
  
  for(i=0; i < bin.secs.size(); i++)
  {
    sec = &bin.secs[i];
    printf("\t0x%016jx %-8ju %-20s %s\n",
            sec->vma,
            sec->size,
            sec->name.c_str(),
            sec->type == Section::SEC_TYPE_CODE ? "CODE":"DATA");

    if(dig && !sec->name.compare(digsection_name))
      digsection = sec;
  }
  
  if(bin.syms.size() > 0)
  {
    printf("\n\nScanned symbol tables:\n");
    for(i=0; i<bin.syms.size(); i++)
    {
      sym = &bin.syms[i];
      printf("\t%-40s 0x%016jx %s\n",
              sym->name.c_str(),
              sym->addr,
              (sym->type & Symbol::SYM_TYPE_FNC) ? "FUNC":"");
    }
  }
  
  if(dig)
  {
    printf("Digging into %s:\n\n",digsection->name.c_str());
    for(i=0; i<digsection->size; i++)
    {   
      if(i%16 == 0)
      {
        printf("\n");
        printf("0x%016jx: ", digsection->vma+i);
      }
      else if(i%4 == 0)
      {
        printf(" ");
      }
      printf("%02x", 0xFF & ((char*)digsection->bytes)[i]);
    }
  }

  printf("\n");
  unload_bin(&bin);
  return 0;
}
