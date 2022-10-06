void _hlt();

void main(){
    while (1){
        _hlt();
    }
}
void _hlt(){
    asm("hlt");
}
