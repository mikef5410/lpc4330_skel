/***********************************************************************/
/*                                                                     */
/*  SYSCALLS.C:  System Calls Remapping                                */
/*  for newlib                                                         */
/*                                                                     */
/***********************************************************************/
#include <stdlib.h>
#include <reent.h>




static char *heap_ptr;          /* Points to current end of the heap.   */
extern char *_heap_start;       /* Setup in our linker script */
extern char *_heap_end;
/************************** _sbrk_r *************************************/
/*  Support function.  Adjusts end of heap to provide more memory to    */
/* memory allocator. Simple and dumb with no sanity checks.             */
/*  struct _reent *r    -- re-entrancy structure, used by newlib to     */
/*                      support multiple threads of operation.          */
/*  ptrdiff_t nbytes    -- number of bytes to add.                      */
/*  Returns pointer to start of new heap area.                          */
/*  Note:  This implementation is not thread safe (despite taking a     */
/* _reent structure as a parameter).                                    */
/*  Since _s_r is not used in the current implementation, the following */
/* messages must be suppressed.                                         */

void * _sbrk_r(
    struct _reent *_s_r, 
    ptrdiff_t nbytes)
{
        char  *base;            /*  errno should be set to  ENOMEM on error     */

        if (!heap_ptr) {        /*  Initialize if first time through.           */
                heap_ptr = _heap_start;
        }
        base = heap_ptr;        /*  Point to end of heap.                       */

        if (heap_ptr + nbytes > _heap_end) {
          return((void *) -1);            /* Failure out of mem, We should set errno to ENOMEM, too */
        }

        heap_ptr += nbytes;     /*  Increase heap.                              */
        
        return base;            /*  Return pointer to start of new heap area.   */
}
