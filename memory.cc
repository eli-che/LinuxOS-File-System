
Kernel k;
uint32_t Frame = k.userModeHeap.base;

void Kernel::syscall(int num, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{

    printf("output: syscall %d arg0=%08x arg1=%08x arg2=%08x arg3=%08x\n", num, arg0, arg1, arg2, arg3);
    switch(num) {
        case SYS_BRK:
        {
          uint32_t Dir = ((userModeHeap.base + arg0) >> 22); // First 10 Bits
          uint32_t Page = ((userModeHeap.base + arg0) >> 12) & 0x3ff; // 10 middle bits

            PageTable *Table;
            if(mmu.cr3.tables[Dir] == NULL) { Table = new PageTable(); } //does table exist?
            else { Table = mmu.cr3.tables[Dir]; } // If it exists, add the directory path

            uint32_t counter = arg0 / 0x00001000; // How many frames do we need?


            for(uint32_t i = 0; i < counter; i++) // allocate a frame for each loop
            {



              if (arg0 + 0x00200000 > Frame) {
                Dir = (Frame >> 22);
                Page = (Frame >> 12) & 0x3ff;

                if(mmu.cr3.tables[Dir] == NULL) //If directory doesnt have table, make it point to one
                {
                  mmu.cr3.tables[Dir] = Table;
                }

                if(Table->entries[Page].present == false)
                {
                  PageTableEntry theEntry = PageTableEntry();
                  theEntry.present = true; //Tell the table that frame exist
                  theEntry.frame = allocateFrame(); //Allocate a frame
                  Table->entries[Page] = theEntry; //Make the table point to a frame
                }
              }




              else if (arg0 + 0x00200000 < Frame) {
                freeFrame(mmu.cr3.tables[Dir]->entries[Page].frame);
                Frame -= 0x00001000;
                std::cout << "Frame: " << Frame << std::endl;
                if(arg0 == 0x00001000 ) {
                mmu.cr3.tables[Dir]->entries[Page].present = false;
              }

              }

              Frame += 0x00001000;

          }
          return;
      }
        default:
            std::cout << "Unrecognised syscall: " << num << std::endl;
            return;
  }
}


void Kernel::pageFaultHandler(uint32_t vaddr)
{
  printf("output: Page fault! addr=%08x\n",vaddr);

  if (vaddr <= 4294967295 && vaddr >= 4294930431) {
  uint32_t Dir = ((vaddr) >> 22); // First 10 Bits
  uint32_t Page = ((vaddr) >> 12) & 0x3ff; // 10 middle bits
  PageTable *Table;
  if(mmu.cr3.tables[Dir] == NULL) { Table = new PageTable(); } //does table exist?
  else { Table = mmu.cr3.tables[Dir]; } // If it exists, add the directory path

    if(mmu.cr3.tables[Dir] == NULL) //If directory doesnt have table, make it point to one
    {
      mmu.cr3.tables[Dir] = Table;
    }

    if(Table->entries[Page].present == false)
    {
      PageTableEntry theEntry = PageTableEntry();
      theEntry.present = true; //Tell the table that frame exist
      theEntry.frame = allocateFrame(); //Allocate a frame
      Table->entries[Page] = theEntry; //Make the table point to a frame
    }
}




  //  std::cout << "Haha: " << vaddr << std::endl;
  else { throw std::string("output: Program segv"); }
}
