/*===-- llvm-c/Object.h - Object Lib C Iface --------------------*- C++ -*-===*/
/*                                                                            */
/*                     The LLVM Compiler Infrastructure                       */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/
/*                                                                            */
/* This header declares the C interface to libLLVMObject.a, which             */
/* implements object file reading and writing.                                */
/*                                                                            */
/* Many exotic languages can interoperate with C code but have a harder time  */
/* with C++ due to name mangling. So in addition to C, this interface enables */
/* tools written in such languages.                                           */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

#ifndef LLVM_C_OBJECT_H
#define LLVM_C_OBJECT_H

#include "llvm-c/Core.h"
#include "llvm/Config/llvm-config.h"

#ifdef __cplusplus
#include "llvm/Object/ObjectFile.h"

extern "C" {
#endif

/**
 * @defgroup LLVMCObject Object file reading and writing
 * @ingroup LLVMC
 *
 * @{
 */
typedef enum {
  LLVMSymbolUnknown,
  LLVMSymbolData,
  LLVMSymbolDebug,
  LLVMSymbolFile,
  LLVMSymbolFunction,
  LLVMSymbolOther
} LLVMObjectFileSymbolType;

typedef enum {
  LLVMSymbolFlagNone           = 0,
  /** Symbol is defined in another object file. */
  LLVMSymbolFlagUndefined      = 1U << 0,
  /** Global symbol. */
  LLVMSymbolFlagGlobal         = 1U << 1,
  /** Weak symbol. */
  LLVMSymbolFlagWeak           = 1U << 2,
  /** Absolute symbol. */
  LLVMSymbolFlagAbsolute       = 1U << 3,
  /** Thread local symbol. */
  LLVMSymbolFlagThreadLocal    = 1U << 4,
  /** Symbol has common linkage. */
  LLVMSymbolFlagCommon         = 1U << 5,
  /** Specific to the object file format (e.g. section symbols). */
  LLVMSymbolFlagFormatSpecific = 1U << 31
} LLVMObjectFileSymbolFlags;

// Opaque type wrappers
typedef struct LLVMOpaqueObjectFile *LLVMObjectFileRef;
typedef struct LLVMOpaqueNeededLibraryIterator *LLVMNeededLibraryIteratorRef;
typedef struct LLVMOpaqueSectionIterator *LLVMSectionIteratorRef;
typedef struct LLVMOpaqueSymbolIterator *LLVMSymbolIteratorRef;
typedef struct LLVMOpaqueRelocationIterator *LLVMRelocationIteratorRef;

// ObjectFile creation
LLVMObjectFileRef LLVMCreateObjectFile(LLVMMemoryBufferRef MemBuf);
void LLVMDisposeObjectFile(LLVMObjectFileRef ObjectFile);

// ObjectFile Section iterators
LLVMSectionIteratorRef LLVMGetSections(LLVMObjectFileRef ObjectFile);
void LLVMDisposeSectionIterator(LLVMSectionIteratorRef SI);
LLVMBool LLVMIsSectionIteratorAtEnd(LLVMObjectFileRef ObjectFile,
                                LLVMSectionIteratorRef SI);
void LLVMMoveToNextSection(LLVMSectionIteratorRef SI);
void LLVMMoveToContainingSection(LLVMSectionIteratorRef Sect,
                                 LLVMSymbolIteratorRef Sym);

// ObjectFile Symbol iterators
LLVMSymbolIteratorRef LLVMGetSymbols(LLVMObjectFileRef ObjectFile);
void LLVMDisposeSymbolIterator(LLVMSymbolIteratorRef SI);
LLVMBool LLVMIsSymbolIteratorAtEnd(LLVMObjectFileRef ObjectFile,
                                LLVMSymbolIteratorRef SI);
void LLVMMoveToNextSymbol(LLVMSymbolIteratorRef SI);

// SectionRef accessors
const char *LLVMGetSectionName(LLVMSectionIteratorRef SI);
uint64_t LLVMGetSectionSize(LLVMSectionIteratorRef SI);
const char *LLVMGetSectionContents(LLVMSectionIteratorRef SI);
uint64_t LLVMGetSectionAddress(LLVMSectionIteratorRef SI);
LLVMBool LLVMGetSectionContainsSymbol(LLVMSectionIteratorRef SI,
                                 LLVMSymbolIteratorRef Sym);

/**
 * Obtain the alignment of a section as the actual value (not log 2).
 */
uint64_t LLVMGetSectionAlignment(LLVMSectionIteratorRef SI);

/**
 * Whether a section is a text section.
 */
LLVMBool LLVMSectionIsText(LLVMSectionIteratorRef SI);

/**
 * Whether a section is a data section.
 */
LLVMBool LLVMSectionIsData(LLVMSectionIteratorRef SI);

/**
 * Whether a section is a BSS Section.
 *
 * BSS sections typically contain statically allocated variables.
 */
LLVMBool LLVMSectionIsBSS(LLVMSectionIteratorRef SI);

// Section Relocation iterators
LLVMRelocationIteratorRef LLVMGetRelocations(LLVMSectionIteratorRef Section);
void LLVMDisposeRelocationIterator(LLVMRelocationIteratorRef RI);
LLVMBool LLVMIsRelocationIteratorAtEnd(LLVMSectionIteratorRef Section,
                                       LLVMRelocationIteratorRef RI);
void LLVMMoveToNextRelocation(LLVMRelocationIteratorRef RI);


// SymbolRef accessors
const char *LLVMGetSymbolName(LLVMSymbolIteratorRef SI);
uint64_t LLVMGetSymbolAddress(LLVMSymbolIteratorRef SI);
uint64_t LLVMGetSymbolFileOffset(LLVMSymbolIteratorRef SI);
uint64_t LLVMGetSymbolSize(LLVMSymbolIteratorRef SI);

/**
 * Obtain the type of a symbol in an ObjectFile.
 */
LLVMObjectFileSymbolType LLVMGetSymbolType(LLVMSymbolIteratorRef SI);

/**
 * Obtain the ASCII character this symbol would be represented with in nm.
 */
char LLVMGetSymbolNMTypeChar(LLVMSymbolIteratorRef SI);

/**
 * Obtain symbol flags for a symbol.
 *
 * This returns a bitwise OR of LLVMObjectFileSymbolFlags.
 */
LLVMObjectFileSymbolFlags LLVMGetSymbolFlags(LLVMSymbolIteratorRef SI);

// RelocationRef accessors
uint64_t LLVMGetRelocationAddress(LLVMRelocationIteratorRef RI);
uint64_t LLVMGetRelocationOffset(LLVMRelocationIteratorRef RI);
LLVMSymbolIteratorRef LLVMGetRelocationSymbol(LLVMRelocationIteratorRef RI);
uint64_t LLVMGetRelocationType(LLVMRelocationIteratorRef RI);
// NOTE: Caller takes ownership of returned string of the two
// following functions.
const char *LLVMGetRelocationTypeName(LLVMRelocationIteratorRef RI);
const char *LLVMGetRelocationValueString(LLVMRelocationIteratorRef RI);

// Library iterators.

/**
 * Obtain an iterator over libraries required by this ObjectFile.
 */
LLVMNeededLibraryIteratorRef
  LLVMGetNeededLibraries(LLVMObjectFileRef ObjectFile);

void LLVMDisposeNeededLibraryIterator(LLVMNeededLibraryIteratorRef NLI);

/**
 * Determine whether a needed library iterator is at the end of the list.
 */
LLVMBool LLVMNeededLibraryIteratorAtEnd(LLVMObjectFileRef ObjectFile,
                                        LLVMNeededLibraryIteratorRef NLI);

/**
 * Advance the needed library iterator to the next element.
 */
void LLVMMoveToNextNeededLibrary(LLVMNeededLibraryIteratorRef NLI);

// Library accessors.

/**
 * Obtain the path of the needed library.
 */
const char * LLVMGetNeededLibraryPath(LLVMNeededLibraryIteratorRef NLI);

/**
 * @}
 */

#ifdef __cplusplus
}

namespace llvm {
  namespace object {
    inline ObjectFile *unwrap(LLVMObjectFileRef OF) {
      return reinterpret_cast<ObjectFile*>(OF);
    }

    inline LLVMObjectFileRef wrap(const ObjectFile *OF) {
      return reinterpret_cast<LLVMObjectFileRef>(const_cast<ObjectFile*>(OF));
    }

    inline section_iterator *unwrap(LLVMSectionIteratorRef SI) {
      return reinterpret_cast<section_iterator*>(SI);
    }

    inline LLVMSectionIteratorRef
    wrap(const section_iterator *SI) {
      return reinterpret_cast<LLVMSectionIteratorRef>
        (const_cast<section_iterator*>(SI));
    }

    inline symbol_iterator *unwrap(LLVMSymbolIteratorRef SI) {
      return reinterpret_cast<symbol_iterator*>(SI);
    }

    inline LLVMSymbolIteratorRef
    wrap(const symbol_iterator *SI) {
      return reinterpret_cast<LLVMSymbolIteratorRef>
        (const_cast<symbol_iterator*>(SI));
    }

    inline relocation_iterator *unwrap(LLVMRelocationIteratorRef SI) {
      return reinterpret_cast<relocation_iterator*>(SI);
    }

    inline LLVMRelocationIteratorRef
    wrap(const relocation_iterator *SI) {
      return reinterpret_cast<LLVMRelocationIteratorRef>
        (const_cast<relocation_iterator*>(SI));
    }

    inline LLVMNeededLibraryIteratorRef wrap(const library_iterator *LI) {
      return reinterpret_cast<LLVMNeededLibraryIteratorRef>
        (const_cast<library_iterator *>(LI));
    }

    inline library_iterator *unwrap(LLVMNeededLibraryIteratorRef NLI) {
      return reinterpret_cast<library_iterator*>(NLI);
    }
  }
}

#endif /* defined(__cplusplus) */

#endif

