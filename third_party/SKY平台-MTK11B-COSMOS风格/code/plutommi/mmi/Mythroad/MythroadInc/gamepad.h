#ifndef __EV_GAMEKEY__
#define __EV_GAMEKEY__

//when start run to simulate a NES game, call it to start
//scan external keypad device
extern void EV_EntryGameSimu(void);

//when exit from a NES game, call it to stop external keypad device scan
extern void EV_ExitGameSimu(void);

//get external keypad device bit map, the low 16bit is valid, the upper is reserved
//except that the MSB is used as external device flag
extern kal_uint32 EV_GameSimuKeyValue(void);

#define EV_KEYDEV_PLUGIN 	0x80000000
#define EV_KEYDEV_PLUGOUT 	0x00000000

/*******
	0x0001,	//up     
	0x0002,	//down   
	0x0004,	//left   
	0x0008,	//right  
	0x0200,	//A      
	0x0400,	//B      
	0x0800,	//X      
	0x1000,	//Y      
	0x0040,	//select 
	0x0080,	//OK     
	0x0010,	//TOPL   
	0x0020,	//TOPR   
	0x0000,	//MUTE	 
	0x0000,	//ESC    
	0x0100	//MENU   
********/	

#endif //__EV_GAMEKEY__

/*#auto end#*/