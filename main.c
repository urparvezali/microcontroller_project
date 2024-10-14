void init(){
	trisb = 0x00;
	trisc = 0x00;
	trisd = 0xff;
	trisa = 0x00;
	portb = 0x00;
	portc = 0x00;
	portd = 0x00;
	porta = 0x00;
}

unsigned int arr[] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F };
unsigned int digit1 = 0;
unsigned int digit0 = 0;
unsigned int mode = 1; // 0=run_mode, 1=program_mode
unsigned int on_time = 0;
unsigned int off_time = 0;
unsigned int channels = 0;

unsigned int read_int(unsigned int add){
	return EEPROM_Read(add) | (EEPROM_Read(add+1) << 8);
}
void write_int(unsigned int add, unsigned int val){
	EEPROM_Write(add, (val & 0xff));
	EEPROM_Write(add+1, ((val >> 8) & 0xff));
}

void get_mode(){
	delay_ms(100);
	if (portd.f0){
		mode = 1;
	} else{
		mode = 0;
	}
}
void read_rom(){
	on_time = read_int(0x00);
	off_time = read_int(0x02);
	channels = read_int(0x04);
}
void write_rom(){
	write_int(0x00, on_time);
	write_int(0x02, off_time);
	write_int(0x04, channels);
}
void show_led(int value){
	digit1 = value%10;
	digit0 = value/10;

	portc.f1 = 1;
	portb = arr[digit0];
	portc.f0 = 0;

	delay_ms(5);

	portc.f0 = 1;
	portb = arr[digit1];
	portc.f1 = 0;

	delay_ms(5);
}

void program_mode(){
	on_time = 0;
	off_time = 0;
	channels = 0;
	// for changing on time
	while (1){
		// detect change of state
		if (portd.f1){
			while (portd.f1) delay_ms(5);
			break;
		}
		if (portd.f2){
			while (portd.f2) delay_ms(5);
			on_time += on_time<99 ? 1 : 0;
		}
		if (portd.f3){
			while (portd.f3) delay_ms(5);
			on_time -= on_time>0 ? 1 : 0;
		}
		show_led(on_time);
	}
	// for changing off time
	while (1){
		// detect change of state
		if (portd.f1){
			while (portd.f1) delay_ms(5);
			break;
		}
		if (portd.f2){
			while (portd.f2) delay_ms(5);
			off_time += 1;
		}
		if (portd.f3){
			while (portd.f3) delay_ms(5);
			off_time -= off_time>0 ? 1 : 0;
		}
		show_led(off_time);
	}
	// for changing channels
	while (1){
		// detect change of state
		if (portd.f1){
			while (portd.f1) delay_ms(5);
			break;
		}
		if (portd.f2){
			while (portd.f2) delay_ms(5);
			channels += channels<16 ? 1 : 0;
		}
		if (portd.f3){
			while (portd.f3) delay_ms(5);
			channels -= channels>0 ? 1 : 0;
		}
		show_led(channels);
	}
	write_rom();
}

void run_mode(){
	unsigned int i = 0;
	unsigned int j = 0;
	portc.f2 = 1;

	while (1){
		for (i = 0; i<channels; i++){
			portc.f2 = 1;
			porta = i;
			for (j = 0; j<on_time; j++){
				delay_ms(100);
			}
			portc.f2 = 0;
			for (j = 0; j<off_time; j++){
				delay_ms(1000);
			}
		}
	}
}

void main(){
	init();
	get_mode();
	read_rom();
	if (mode){
		program_mode();
	}
	run_mode();
}