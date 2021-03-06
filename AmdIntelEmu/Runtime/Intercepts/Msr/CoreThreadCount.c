#include <Base.h>

#include <Register/Msr.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include "../../AmdIntelEmuRuntime.h"

VOID
AmdIntelEmuInternalRdmsrCoreThreadCount (
  IN OUT AMD_VMCB_SAVE_STATE_AREA_NON_ES  *SaveState,
  IN OUT AMD_INTEL_EMU_REGISTERS          *Registers,
  OUT    AMD_VMCB_CLEAN_FIELD             *CleanField
  )
{
  MSR_HASWELL_E_CORE_THREAD_COUNT_REGISTER CoreThreadCountMsr;
  CPUID_AMD_APIC_ID_SIZE_CORE_COUNT_ECX    CoreCountEcx;
  CPUID_AMD_COMPUTE_UNIT_IDENTIFIERS_EBX   CuIdEbx;
  UINT16                                   ThreadCount;
  UINT16                                   CoreCount;

  ASSERT (SaveState != NULL);
  ASSERT (Registers != NULL);

  AsmCpuid (
    CPUID_AMD_APIC_ID_SIZE_CORE_COUNT,
    NULL,
    NULL,
    &CoreCountEcx.Uint32,
    NULL
    );
  ThreadCount = (UINT16)(CoreCountEcx.Bits.NC + 1);

  AsmCpuid (
    CPUID_AMD_COMPUTE_UNIT_IDENTIFIERS,
    NULL,
    &CuIdEbx.Uint32,
    NULL,
    NULL
    );
  CoreCount = (ThreadCount / (UINT16)(CuIdEbx.Bits.CoresPerComputeUnit + 1));
  //
  // XNU has Core_Count and Thread_Count swapped.
  //
  CoreThreadCountMsr.Uint64            = 0;
  CoreThreadCountMsr.Bits.Core_Count   = ThreadCount;
  CoreThreadCountMsr.Bits.Thread_Count = CoreCount;
  AmdIntelEmuInternalWriteMsrValue64 (
    &SaveState->RAX,
    Registers,
    CoreThreadCountMsr.Uint64
    );
}

VOID
AmdIntelEmuInternalWrmsrCoreThreadCount (
  IN OUT AMD_VMCB_SAVE_STATE_AREA_NON_ES  *SaveState,
  IN OUT AMD_INTEL_EMU_REGISTERS          *Registers,
  OUT    AMD_VMCB_CLEAN_FIELD             *CleanField
  )
{
  //
  // THREAD_CORE_COUNT is read-only.
  //
}
