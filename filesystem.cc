
FileSystem::FileSystem(Disk & d): disk(d) {}

intptr_t FileSystem::defaultCWD() {
  uint8_t * root = new uint8_t[4096];
  disk.read(baseblock, root);
  baseBuffer = root;
  return (intptr_t) baseBuffer;
}

void FileSystem::bitMove(uint8_t * buffer, uint32_t address, uint32_t index) {
  uint32_t num = 8;
  for (int i = 0; i < 4; i++) {
    num = 8 * i;
    buffer[index + i] = (address >> (unsigned int) num) & 0xFF;

  }
}
uint32_t FileSystem::bitMove(uint8_t * buffer, uint32_t index) {
  uint32_t num = 8;
  uint32_t address = 0;
  address |= buffer[index];
  for (int i = 1; i < 4; i++) {
    num = 8 * i;
    address |= buffer[index + i] << num;
  }
  return address;
}

void FileSystem::format() {
  uint32_t num = 0;
  baseBuffer[0] = 1;
  for (uint32_t i = 1; i < 4096; i++) {
    baseBuffer[i] = 0;
  }
  disk.write(baseblock, baseBuffer);
  uint8_t * Table_x = new uint8_t[4096];
  for (uint32_t i = 0; i < 4096; i++) {
    Table_x[i] = 0;
  }
  for (int i = 0; i < 3; i++) {
    num = i * 4;
    bitMove(Table_x, 1, num);
  }
  disk.write(Tableblock_x, Table_x);
  uint8_t * fileTable = new uint8_t[4096];
  for (uint32_t i = 0; i < 4096; i++) {
    fileTable[i] = 0;
  }
  disk.write(Tableblock_y, fileTable);
  delete[] Table_x;
  delete[] fileTable;
}

void FileSystem::dump() {
  TraverseFileSystem("/", baseblock);
}

void FileSystem::TraverseFileSystem(std::string dirName, uint32_t block) {
  uint8_t * Cblcok = new uint8_t[4096], * Table_x = new uint8_t[4096];
  disk.read(block, Cblcok);
  disk.read(Tableblock_x, Table_x);
  if ((uint32_t) Cblcok[0] == 1) {
    std::vector < std::string > names;
    std::vector < uint32_t > blocks;
    uint32_t index = 1;
    while (true) {
      uint32_t nameSize = (uint32_t) Cblcok[index];
      if (nameSize > 0) {
        index++;
        std::string name = "";
        uint32_t loopMax = index + nameSize;
        for (uint32_t i = index; i < loopMax; i++) {
          name += Cblcok[i];
          index++;
        }
        names.push_back("/" + name);
        blocks.push_back(bitMove(Cblcok, index));
        index += 3;
      }
      index++;
      if (index >= 4089) {
        break;
      }
    }
    std::cout << "Directory path " + dirName + " contains " + std::to_string(names.size()) + " files\n";
    for (uint32_t i = 0; i < names.size(); i++) {
      TraverseFileSystem(names[i], blocks[i]);
    }
  } else {
    uint32_t bytes = bitMove(Cblcok, 1);
    while (true) {
      if (Table_x[block * 4] == 1) {
        std::cout << "File " + dirName + " contains " + std::to_string(bytes) + " bytes\n";
        break;
      } else {
        bytes = sizeLookUp(Table_x, Table_x[block * 4], bytes);
        std::cout << "File " + dirName + " contains " + std::to_string(bytes) + " bytes\n";
        break;
      }
    }
  }
  delete[] Cblcok;
  delete[] Table_x;
}

uint32_t FileSystem::sizeLookUp(uint8_t * Table_x, uint32_t block, uint32_t bytes) {
  uint8_t * currentFile = new uint8_t[4096];
  disk.read(block, currentFile);
  bytes += bitMove(currentFile, 1);
  if (Table_x[block * 4] != 1) {
    bytes = sizeLookUp(Table_x, Table_x[block * 4], bytes);
  }
  delete[] currentFile;
  return bytes;
}

int FileSystem::open(intptr_t CWD,
  const char * path, uint32_t flags) {
  std::cout << "open(" << CWD << ", " << path << ", " << flags << ")" << std::endl;
  uint8_t * dir = (uint8_t * ) CWD;
  uint32_t index = 0;
  std::string name = "";
  for (uint32_t i = 0; i < strlen(path); i++) {
    name += path[i];
  }
  bool done = false;
  if (dir[index] == 1) {
    while (done == false) {
      index++;
      uint32_t nameSize = dir[index];
      index++;
      if (nameSize != 0) {
        std::string currentName = "";
        uint32_t maxloop = index + nameSize;
        for (uint32_t i = index; i < maxloop; i++) {
          currentName += dir[i];
          index++;
        }
        if (currentName == name) {
          return bitMove(dir, index);
        }
        index += 3;
        if (index >= 4089) {
          done = true;
        }
      }
    }
  }
  if (flags == 1 || flags == 3) {
    uint8_t * Table_x = new uint8_t[4096];
    disk.read(Tableblock_x, Table_x);
    bool found = false;
    uint32_t ind;
    for (uint32_t i = 0; i < 4096; i += 4) {
      uint32_t blockIndex = bitMove(Table_x, i);
      if (blockIndex == 0) {
        ind = i / 4;
        found = true;
        i = 4096;
      }
    }
    if (found == true) {

      uint32_t table1Index = (ind * 4), dirIndex = 0, nameSizeIndex, nameSize, maxloop, pathIndex = 0;
      while (true) {
        dirIndex = dirIndex + 1;
        if ((uint32_t) dir[dirIndex] == 0) {
          nameSize = strlen(path);
          nameSizeIndex = dirIndex;
          dirIndex++;
          maxloop = (dirIndex + nameSize);
          for (uint32_t i = dirIndex; i < maxloop; i++) {
            dirIndex++;
          }
          dirIndex += 3;
          break;
        } else {
          nameSize = dir[dirIndex];
          dirIndex++;
          maxloop = (dirIndex + nameSize);
          for (uint32_t i = dirIndex; i < maxloop; i++) {
            dirIndex++;
          }
          dirIndex += 3;
        }
      }
      dir[nameSizeIndex] = (uint8_t) strlen(path);
      dirIndex++;
      uint32_t nameIndexes = nameSizeIndex + 1 + strlen(path);
      for (uint32_t i = nameSizeIndex + 1; i < nameIndexes; i++) {
        dir[i] = path[pathIndex];
        pathIndex++;
      }
      uint8_t * file = new uint8_t[4096];
      file[0] = 0;
      bitMove(file, 0, 1);
      bitMove(dir, ind, nameIndexes);
      bitMove(Table_x, 1, table1Index);
      disk.write(baseblock, dir);
      disk.write(Tableblock_x, Table_x);
      disk.write(ind, file);
      delete[] Table_x;
      delete[] file;
      return ind;

    }
  }
  return -1;
}

void FileSystem::write(uint32_t fd, uint8_t * buffer, uint32_t size, uint32_t bufferindex) {
  uint8_t * file = new uint8_t[4096];
  uint8_t * Table_x = new uint8_t[4096];
  disk.read(fd, file);
  disk.read(Tableblock_x, Table_x);
  uint32_t cSize = bitMove(file, 1);
  uint32_t nSize = size - bufferindex;
  uint32_t minloop = cSize + 5, maxloop = nSize + 5;

  if (maxloop > 4096)
    maxloop = 4096;

  for (uint32_t i = minloop; i < maxloop; i++) {
    file[i] = buffer[bufferindex];
    cSize++;
    bufferindex++;
  }

  bitMove(file, cSize, 1);
  disk.write(fd, file);

  if (bufferindex == size) {
    return;
  }
  if (bitMove(Table_x, fd * 4) == 1) {
    for (uint32_t i = 12; i < 4096; i += 4) {
      if (bitMove(Table_x, i) == 0) {
        uint8_t * nextfile = new uint8_t[4096];
        bitMove(Table_x, 1, i);
        bitMove(Table_x, i / 4, fd * 4);
        nextfile[0] = 0;
        bitMove(nextfile, 0, 1);
        disk.write(i / 4, nextfile);
        disk.write(Tableblock_x, Table_x);
        write(i / 4, buffer, size, bufferindex);
        delete[] file;
        delete[] Table_x;
        delete[] nextfile;
        return;
      }
    }
  } else {
    write(bitMove(Table_x, fd * 4), buffer, size, bufferindex);
    delete[] file;
    delete[] Table_x;
    return;
  }
}

