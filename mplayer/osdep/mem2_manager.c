#include <ogc/machine/asm.h>
#include <ogc/lwp_heap.h>
#include <ogc/system.h>
#include <ogc/machine/processor.h>


static heap_cntrl mem2_heap;
static bool mem2_initied=false;
static u32 mem2_size=0;
//u32 MALLOC_MEM2=1;  // to disable/enable sbrk.c mem2 management

u32 InitMem2Manager (u32 size) 
{
	u32 level;
	unsigned char *mem2_heap_ptr;
	if(mem2_initied) return mem2_size;
	_CPU_ISR_Disable(level);
	//size = (u32)SYS_GetArena2Hi() - (u32)SYS_GetArena2Lo() - (1024*1024);  //we reserved 1MB at end (for example for usb2), we have about 63MB
	//size = 16*1024*1024; //16Mb
	
	size &= ~0x1f;          //round down, because otherwise we may exceed the area

	mem2_heap_ptr = (u32)SYS_GetArena2Hi()-size;

	//SYS_SetArena2Lo(mem2_heap_ptr + size);
	
	SYS_SetArena2Hi(mem2_heap_ptr );
	//SYS_SetArena2Lo(mem2_heap_ptr + size);
	_CPU_ISR_Restore(level);

	size = __lwp_heap_init(&mem2_heap, mem2_heap_ptr, size, 32);
	mem2_initied=true;
	mem2_size=size;
	return size;
}

void *mem2_malloc(u32 size)
{
	if(mem2_initied) return __lwp_heap_allocate(&mem2_heap, size);
	else return malloc(size);
}

void *mem2_align(u8 align, u32 size, heap_cntrl *heap_ctrl)
{
	// memalign hack from libavutil/mem.c
	void *pointer = __lwp_heap_allocate(heap_ctrl, size + align);
	long difference;
	
	if (!pointer)
		return NULL;
	
	difference = ((-(long)pointer - 1) & (align - 1)) + 1;
	pointer = (char *)pointer + difference;
	((char *)pointer)[-1] = difference;
	
	return pointer;
}

bool mem2_free(void *pointer, bool aligned, heap_cntrl *heap_ctrl)
{
	if (aligned)
		return __lwp_heap_free(heap_ctrl, (char *)pointer - ((char *)pointer)[-1]);
	else
		return __lwp_heap_free(heap_ctrl, pointer);
}