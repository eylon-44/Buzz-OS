void kernel_main() {
	// [Testing] Put the character "X" on the screen using video memory manipulation
	char* video_memory = (char*) 0xb8000;
	*video_memory = 'X';
}
