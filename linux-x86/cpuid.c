/* 
   CPUID instruction extended processor information example
   ========================================================
   To use the brand string method, execute CPUID with EAX input of 8000002H through 
   80000004H. For each input value, CPUID returns 16 ASCII characters using EAX, 
   EBX, ECX, and EDX. The returned string will be NULL-terminated. Compiles on 32
   or 64bit hosts. 

   hackerfantastic$ ./cpuid 
   cpuid instruction extended index upto 80000008
   Intel(R) Core(TM) i7-4960HQ CPU @ 2.60GHz

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __x86_64__
#define SIZEBUF 33
struct cpuret {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
};
#else
#define SIZEBUF 17
struct cpuret {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
};
#endif

struct cpuret cpuid(int loop){
	struct cpuret ret;
#ifdef __x86_64__
	uint64_t rax,rbx,rcx,rdx;
	asm("xor %rax,%rax");
	switch(loop){
		case 2:
			asm("movq $0x80000002,%rax");
			break;
		case 3:
			asm("movq $0x80000003,%rax");
			break;
		case 4:
			asm("movq $0x80000004,%rax");
			break;
		default:
			break;
	}
	asm("cpuid");
	asm("movq %%rax,%0" : "=r" (rax));
	asm("movq %%rbx,%0" : "=r" (rbx));
	asm("movq %%rcx,%0" : "=r" (rcx));
	asm("movq %%rdx,%0" : "=r" (rdx));
	ret.rax = rax;
	ret.rbx = rbx;
	ret.rcx = rcx;
	ret.rdx = rdx;
#else
	uint32_t eax,ebx,ecx,edx;
	asm("xor %eax,%eax");
	switch(loop){
		case 2:
			asm("movl $0x80000002,%eax");
			break;
		case 3:
			asm("movl $0x80000003,%eax");
			break;
		case 4:
			asm("movl $0x80000004,%eax");
			break;
		default:
			break;
	}
	asm("cpuid");
	asm("movl %%eax,%0" : "=r" (eax));
	asm("movl %%ebx,%0" : "=r" (ebx));
	asm("movl %%ecx,%0" : "=r" (ecx));
	asm("movl %%edx,%0" : "=r" (edx));
	ret.eax = eax;
	ret.ebx = ebx;
	ret.ecx = ecx;
	ret.edx = edx;
#endif
	return ret;
}

#ifdef __x86_64__
uint64_t supported(){
	uint64_t rax;
	asm("xor %rax,%rax");
	asm("movq $0x80000000, %rax");
	asm("cpuid");
	asm("movq %%rax,%0" : "=r" (rax));
	return rax;
}
#else
uint32_t supported(){
	uint32_t eax;
	asm("xor %eax,%eax");
	asm("movl $0x80000000, %eax");
	asm("cpuid");
	asm("movl %%eax,%0" : "=r" (eax));
	return eax;
}
#endif

int main(int argc, char* argv[]){
#ifdef __x86_64__
	uint64_t ptr,rax,rbx,rcx,rdx;
#else
	uint32_t ptr,eax,ebx,ecx,edx;
#endif
	char* buf = malloc(SIZEBUF);
	if(!buf){
		printf("Error in malloc();\n");
		exit(0);
	}
	int loop = 1;	
	struct cpuret ret;
#ifdef __x86_64__
	rax = supported();
	if(!(rax & 0x80000000)){
		printf("Not supported on this processor\n");
		return 0;
	}
	else{
		printf("cpuid instruction extended index upto %llx\n", rax);
	}
	if(buf){
		for(loop = 2;loop <= 4;loop++){
			ret = cpuid(loop);
			memset(buf,0,SIZEBUF);
			ptr = (uint64_t)buf;
			memcpy((void*)ptr,(void*)&ret.rax,8);
			ptr+=4;
			memcpy((void*)ptr,(void*)&ret.rbx,8);
			ptr+=4;
			memcpy((void*)ptr,(void*)&ret.rcx,8);
			ptr+=4;
			memcpy((void*)ptr,(void*)&ret.rdx,8);
			ptr = (uint64_t)buf;
			printf("%s",buf);
		}
	}
#else
	eax = supported();
	if(!(eax & 0x80000000)){
		printf("Not supported on this processor\n");
		return 0;
	}
	else{
		printf("cpuid instruction extended index upto %x\n", eax);
	}
	if(buf){
		for(loop = 2;loop <= 4;loop++){
			ret = cpuid(loop);
			memset(buf,0,SIZEBUF);
			ptr = (uint32_t)buf;
			memcpy((void*)ptr,(void*)&ret.eax,4);
			ptr+=4;
			memcpy((void*)ptr,(void*)&ret.ebx,4);
			ptr+=4;
			memcpy((void*)ptr,(void*)&ret.ecx,4);
			ptr+=4;
			memcpy((void*)ptr,(void*)&ret.edx,4);
			ptr = (uint32_t)buf;
			printf("%s",buf);
		}
	}
#endif
	printf("\n");
	return 0;
}
