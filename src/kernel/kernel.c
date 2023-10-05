// Buzz OS Kernel Main // ~ eylon

void kernel_main() {
	// [Testing] Put the character "P" on the screen using video memory manipulation
	char* video_memory = (char*) 0xb8000;
	*video_memory = 'P';
}