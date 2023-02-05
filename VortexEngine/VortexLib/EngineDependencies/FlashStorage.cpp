/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Written by Cristian Maglie

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "FlashStorage.h"
#include <string.h>

#include <stdio.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif

static const uint32_t pageSizes[] = { 8, 16, 32, 64, 128, 256, 512, 1024 };

// using same values as recorded from Trinket M0: pagesize index 3 and 4096 pages
#define PAGE_SIZE_IDX 3
#define NUM_PAGES 4096

FlashClass::FlashClass(const void *flash_addr, uint32_t size) :
  PAGE_SIZE(pageSizes[PAGE_SIZE_IDX]),
  PAGES(NUM_PAGES),
  MAX_FLASH(PAGE_SIZE * PAGES),
  ROW_SIZE(PAGE_SIZE * 4),
  flash_address((volatile void *)flash_addr),
  flash_size(size)
{
}

static inline uint32_t read_unaligned_uint32(const void *data)
{
  union {
    uint32_t u32;
    uint8_t u8[4];
  } res;
  const uint8_t *d = (const uint8_t *)data;
  res.u8[0] = d[0];
  res.u8[1] = d[1];
  res.u8[2] = d[2];
  res.u8[3] = d[3];
  return res.u32;
}

void FlashClass::write(const volatile void *flash_ptr, const void *data, uint32_t size)
{
#if 0
  printf("Writing:\r\n\t");
  for (uint32_t i = 0; i < size; ++i) {
    printf("%02x ", ((uint8_t *)data)[i]);
    if (i > 0 && ((i + 1) % 32) == 0) {
      printf("\r\n\t");
    }
  }
  printf("\r\n\r\n");
#endif
#ifndef _MSC_VER
  FILE *f = fopen("FlashStorage.flash", "w");
  if (!f) {
    return;
  }
  if (!fwrite(data, sizeof(char), size, f)) {
    return;
  }
  fclose(f);
#else
  HANDLE hFile = CreateFile("FlashStorage.flash", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (!hFile) {
    // error
    return;
  }
  DWORD written = 0;
  if (!WriteFile(hFile, data, size, &written, NULL)) {
    // error
  }
  CloseHandle(hFile);
#endif
}

void FlashClass::erase(const volatile void *flash_ptr, uint32_t size)
{
  const uint8_t *ptr = (const uint8_t *)flash_ptr;
  while (size > ROW_SIZE) {
    erase(ptr);
    ptr += ROW_SIZE;
    size -= ROW_SIZE;
  }
  erase(ptr);
}

void FlashClass::erase(const volatile void *flash_ptr)
{
#if 0
  // ?????
  NVMCTRL->ADDR.reg = ((uint32_t)flash_ptr) / 2;
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
  while (!NVMCTRL->INTFLAG.bit.READY) { }
#endif
}

void FlashClass::read(const volatile void *flash_ptr, void *data, uint32_t size)
{
#ifndef _MSC_VER
  FILE *f = fopen("FlashStorage.flash", "r");
  if (!f) {
    return;
  }
  if (!fread(data, sizeof(char), size, f)) {
    return;
  }
  fclose(f);
#else
  HANDLE hFile = CreateFile("FlashStorage.flash", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (!hFile) {
    // error
    return;
  }
  DWORD bytesRead = 0;
  if (!ReadFile(hFile, data, size, &bytesRead, NULL)) {
    // error
  }
  CloseHandle(hFile);
#endif
  //printf("Read:\r\n\t");
  uint32_t print_amount = size;
  for (uint32_t i = 0; i < print_amount; ++i) {
    if (i == 0) {
      if (!((uint8_t *)data)[i]) {
        //printf("nothing\r\n");
        break;
      } else {
        print_amount = ((uint8_t *)data)[i];
      }
    }
#if 0
    printf("%02x ", ((uint8_t *)data)[i]);
    if (i > 0 && ((i + 1) % 32) == 0) {
      printf("\r\n\t");
    }
#endif
  }
  //printf("\r\n\r\n");

}

