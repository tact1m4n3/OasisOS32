int main() {
    asm volatile("int $0x80" : : "a"(0), "b"("hello from init process\n"));
    return 0;
}
