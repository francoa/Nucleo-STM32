#include "mbed.h"

Serial pc(USBTX, USBRX); // tx, rx

// main() runs in its own thread in the OS
int main(){
	while(true)
	{
		pc.putc(pc.getc());
	}
}