#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "debug.h"
#include "box86context.h"
#include "dynarec.h"
#include "emu/x86emu_private.h"
#include "tools/bridge_private.h"
#include "x86run.h"
#include "x86emu.h"
#include "box86stack.h"
#include "callback.h"
#include "emu/x86run_private.h"
#include "x86trace.h"
#include "dynablock.h"
#include "dynablock_private.h"
#include "dynarec_arm.h"
#include "dynarec_arm_private.h"
#include "dynarec_arm_functions.h"

void arm_popf(x86emu_t* emu)
{
    emu->packed_eflags.x32 = ((Pop(emu) & 0x3F7FD7) & (0xffff-40) ) | 0x2; // mask off res2 and res3 and on res1
    UnpackFlags(emu);
    RESET_FLAGS(emu);
}