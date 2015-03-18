/*
 * (c) Jeron A. Lau
 * jlvm_blib_inpt is a library for making input compatible between different
 * devices.
*/

/* 
 * these are control definitions 
 * These Keys Are Reserved For hack_user_back():
 *   3DS/WiiU: Select
 *   Android: Back
 *   Computer: Escape
 * When these keys are pressed hack_user_back() is called in a loop and then
 * when it is finished and the return value is:
 *   0:  Then it quits
 *   1:  Then it continues the loop
 *   2:  Then it quits the loop, but continues the main loop.
*/
//nintendo
	//Normal buttons
	#define JL_CT_NINT_BTNL 0 //NYI: L button: Up/Left
	#define JL_CT_NINT_BTNR 1 //NYI: R button: Down/Right
	#define JL_CT_NINT_BTNA 2 //NYI: A button: Right
	#define JL_CT_NINT_BTNB 3 //NYI: B button: Down
	#define JL_CT_NINT_BTNX 4 //NYI: X button: Up
	#define JL_CT_NINT_BTNY 5 //NYI: Y button: Left
	#define JL_CT_NINT_STRT 6 //NYI: Start button
	//XY
	#define JL_CT_NINT_STYL 7 //NYI: Stylus
	#define JL_CT_NINT_CPAD 8 //NYI: Circle Pad -or- left joystick (WiiU)
	#define JL_CT_NINT_JSTK 9 //NYI: Cirle Pad + X -or- right joystick (WiiU)
	#define JL_CT_NINT_UDLR 10//NYI: LR buttons (Y if B pressed, X if not)
	#define JL_CT_NINT_DPAD 11//NYI: D-Pad
	#define JL_CT_NINT_ABXY 12//NYI: ABXY buttons
	//Stylus
	#define JL_CT_NINT_SSUP 13 //NYI: Up
	#define JL_CT_NINT_SSDN 14 //NYI: Down
	#define JL_CT_NINT_SSRT 15 //NYI: Right
	#define JL_CT_NINT_SSLT 16 //NYI: Left
	#define JL_CT_NINT_SSUR 17 //NYI: Up Right
	#define JL_CT_NINT_SSDR 18 //NYI: Down Right
	#define JL_CT_NINT_SSUL 19 //NYI: Up Left
	#define JL_CT_NINT_SSDL 20 //NYI: Down Left
	//Cirle Pad -or- left joystick(WiiU)
	#define JL_CT_NINT_CPUP 21 //NYI: Up
	#define JL_CT_NINT_CPDN 22 //NYI: Down
	#define JL_CT_NINT_CPRT 23 //NYI: Right
	#define JL_CT_NINT_CPLT 24 //NYI: Left
	#define JL_CT_NINT_CPUR 25 //NYI: Up Right
	#define JL_CT_NINT_CPDR 26 //NYI: Down Right
	#define JL_CT_NINT_CPUL 27 //NYI: Up Left
	#define JL_CT_NINT_CPDL 28 //NYI: Down Left
	//Cirle Pad + X -or- right joystick (WiiU)
	#define JL_CT_NINT_JSUP 29 //NYI: Up
	#define JL_CT_NINT_JSDN 30 //NYI: Down
	#define JL_CT_NINT_JSRT 31 //NYI: Right
	#define JL_CT_NINT_JSLT 32 //NYI: Left
	#define JL_CT_NINT_JSUR 33 //NYI: Up Right
	#define JL_CT_NINT_JSDR 34 //NYI: Down Right
	#define JL_CT_NINT_JSUL 35 //NYI: Up Left
	#define JL_CT_NINT_JSDL 36 //NYI: Down Left
	//D-Pad
	#define JL_CT_NINT_DPUP 37 //NYI: Up
	#define JL_CT_NINT_DPDN 38 //NYI: Down
	#define JL_CT_NINT_DPRT 39 //NYI: Right
	#define JL_CT_NINT_DPLT 40 //NYI: Left
	#define JL_CT_NINT_DPUR 41 //NYI: Up Right
	#define JL_CT_NINT_DPDR 42 //NYI: Down Right
	#define JL_CT_NINT_DPUL 43 //NYI: Up Left
	#define JL_CT_NINT_DPDL 44 //NYI: Down Left
	#define JL_CT_NINT_MAXX 45
//computer
	//normal buttons
	#define JL_CT_COMP_SCRU 0 //NYI: Scroll Up
	#define JL_CT_COMP_SCRD 1 //NYI: Scroll Down
	/* Scroll Right(Mac only) -or- Ctrl Key + Down -or- page down */
	#define JL_CT_COMP_SCRR 2 //NYI
	/* Scroll Left(Mac only) -or- Ctrl Key + Up -or- page up */
	#define JL_CT_COMP_SCRL 3 //NYI
	#define JL_CT_COMP_KEYW 4 //W Key
	#define JL_CT_COMP_KEYA 5 //A Key
	#define JL_CT_COMP_KEYS 6 //S Key
	#define JL_CT_COMP_KEYD 7 //D Key
	#define JL_CT_COMP_ARUP 8 //Up Arrow Key
	#define JL_CT_COMP_ARDN 9 //Down Arrow Key
	#define JL_CT_COMP_ARRT 10//Right Arrow Key
	#define JL_CT_COMP_ARLT 11//Left Arrow Key
	#define JL_CT_COMP_NUM0 12//NYI: Numpad 0 key
	#define JL_CT_COMP_NUM1 13//NYI: Numpad 1 key
	#define JL_CT_COMP_NUM2 14//NYI: Numpad 2 key
	#define JL_CT_COMP_NUM3 15//NYI: Numpad 3 key
	#define JL_CT_COMP_NUM4 16//NYI: Numpad 4 key
	#define JL_CT_COMP_NUM5 17//NYI: Numpad 5 key
	#define JL_CT_COMP_NUM6 18//NYI: Numpad 6 key
	#define JL_CT_COMP_NUM7 19//NYI: Numpad 7 key
	#define JL_CT_COMP_NUM8 20//NYI: Numpad 8 key
	#define JL_CT_COMP_NUM9 21//NYI: Numpad 9 key
	#define JL_CT_COMP_KEYJ 22//NYI: J key/LEFT
	#define JL_CT_COMP_KEYK 23//NYI: K key/DOWN
	#define JL_CT_COMP_KEYL 24//NYI: L key/RIGHT
	#define JL_CT_COMP_KEYI 25//NYI: I key/UP
	#define JL_CT_COMP_SPAC 26//NYI: Space key
	#define JL_CT_COMP_RETN 27//Return key
	#define JL_CT_COMP_BACK 28//NYI: Backspace key
	#define JL_CT_COMP_KTAB 29//NYI: Tab key
	#define JL_CT_COMP_CLLT 30//NYI: Left Click
	#define JL_CT_COMP_CLRT 31//NYI: Right Click(PC only) -or- Ctr-Click
	#define JL_CT_COMP_CLCR 32//NYI: Middle Click
	#define JL_CT_COMP_MSVW 33//NYI: Mouse Move [ 3D: looking around ]
	#define JL_CT_COMP_MENU 34//Menu Key
	#define JL_CT_COMP_MAXX 35
//android
	//normal buttons
	#define JL_CT_ANDR_BTNA 0 //NYI: Virtual button A
	#define JL_CT_ANDR_BTNB 1 //NYI: Virtual button B
	#define JL_CT_ANDR_BTNX 2 //NYI: Virtual button X
	#define JL_CT_ANDR_BTNY 3 //NYI: Virtual button Y
	#define JL_CT_ANDR_LDUP 4 //NYI: Virtual Left D-Pad Up
	#define JL_CT_ANDR_LDDN 5 //NYI: Virtual Left D-Pad Down
	#define JL_CT_ANDR_LDRT 6 //NYI: Virtual Left D-Pad Right
	#define JL_CT_ANDR_LDLT 7 //NYI: Virtual Left D-Pad Left
	#define JL_CT_ANDR_RDUP 8 //NYI: Virtual Right D-Pad Up
	#define JL_CT_ANDR_RDDN 9 //NYI: Virtual Right D-Pad Down
	#define JL_CT_ANDR_RDRT 10 //NYI: Virtual Right D-Pad Right
	#define JL_CT_ANDR_RDLT 11 //NYI: Virtual Right D-Pad Left
	#define JL_CT_ANDR_LCUP 12 //NYI: Virtual Left Circle Pad Up
	#define JL_CT_ANDR_LCDN 13 //NYI: Virtual Left Circle Pad Down
	#define JL_CT_ANDR_LCRT 14 //NYI: Virtual Left Circle Pad Right
	#define JL_CT_ANDR_LCLT 15 //NYI: Virtual Left Circle Pad Left
	#define JL_CT_ANDR_LCUR 16 //NYI: Virtual Left Circle Pad Up Right
	#define JL_CT_ANDR_LCDR 17 //NYI: Virtual Left Circle Pad Down Right
	#define JL_CT_ANDR_LCUL 18 //NYI: Virtual Left Circle Pad Up Left
	#define JL_CT_ANDR_LCDL 19 //NYI: Virtual Left Circle Pad Down Left
	#define JL_CT_ANDR_RCUP 20 //NYI: Virtual Right Circle Pad Up
	#define JL_CT_ANDR_RCDN 21 //NYI: Virtual Right Circle Pad Down
	#define JL_CT_ANDR_RCRT 22 //NYI: Virtual Right Circle Pad Right
	#define JL_CT_ANDR_RCLT 23 //NYI: Virtual Right Circle Pad Left
	#define JL_CT_ANDR_RCUR 24 //NYI: Virtual Right Circle Pad Up Right
	#define JL_CT_ANDR_RCDR 25 //NYI: Virtual Right Circle Pad Down Right
	#define JL_CT_ANDR_RCUL 26 //NYI: Virtual Right Circle Pad Up Left
	#define JL_CT_ANDR_RCDL 27 //NYI: Virtual Right Circle Pad Down Left
	#define JL_CT_ANDR_DRUP 28 //NYI: Drag Up
	#define JL_CT_ANDR_DRDN 29 //NYI: Drag Down
	#define JL_CT_ANDR_DRRT 30 //NYI: Drag Right
	#define JL_CT_ANDR_DRLT 31 //NYI: Drag Left
	#define JL_CT_ANDR_DRUR 32 //NYI: Drag Up Right
	#define JL_CT_ANDR_DRDR 33 //NYI: Drag Down Right
	#define JL_CT_ANDR_DRUL 34 //NYI: Drag Up Left
	#define JL_CT_ANDR_DRDL 35 //NYI: Drag Down Left
	#define JL_CT_ANDR_TCUP 36 //NYI: Touch Up
	#define JL_CT_ANDR_TCDN 37 //NYI: Touch Down
	#define JL_CT_ANDR_TCRT 38 //NYI: Touch Right
	#define JL_CT_ANDR_TCLT 39 //NYI: Touch Left
	#define JL_CT_ANDR_TCUR 40 //NYI: Touch Up Right
	#define JL_CT_ANDR_TCDR 41 //NYI: Touch Down Right
	#define JL_CT_ANDR_TCUL 42 //NYI: Touch Up Left
	#define JL_CT_ANDR_TCDL 43 //NYI: Touch Down Left
	#define JL_CT_ANDR_TFUP 44 //Touch Far Up
	#define JL_CT_ANDR_TFDN 45 //Touch Far Down
	#define JL_CT_ANDR_TFRT 46 //Touch Far Right
	#define JL_CT_ANDR_TFLT 47 //Touch Far Left
	#define JL_CT_ANDR_TFUR 48 //NYI: Touch Far Up Right
	#define JL_CT_ANDR_TFDR 49 //NYI: Touch Far Down Right
	#define JL_CT_ANDR_TFUL 50 //NYI: Touch Far Up Left
	#define JL_CT_ANDR_TFDL 51 //NYI: Touch Far Down Left
	#define JL_CT_ANDR_TNUP 52 //Touch Close Up
	#define JL_CT_ANDR_TNDN 53 //Touch Close Down
	#define JL_CT_ANDR_TNRT 54 //Touch Close Right
	#define JL_CT_ANDR_TNLT 55 //Touch Close Left
	#define JL_CT_ANDR_TNUR 56 //NYI: Touch Close Up Right
	#define JL_CT_ANDR_TNDR 57 //NYI: Touch Close Down Right
	#define JL_CT_ANDR_TNUL 58 //NYI: Touch Close Up Left
	#define JL_CT_ANDR_TNDL 59 //NYI: Touch Close Down Left
	#define JL_CT_ANDR_TCCR 60 //Touch Center
	#define JL_CT_ANDR_TOUC 61 //Touch(X,Y) 255-0, 382-0
	#define JL_CT_ANDR_TCJS 62 //NYI: Touch(X,Y) 255-0, 255-0
	#define JL_CT_ANDR_MENU 63 //Menu Key
	#define JL_CT_ANDR_MAXX 64

#if PLATFORM == 0 //COMPUTER
	#define JL_CT_ALLP(nintendo, computer, android) computer
#elif PLATFORM == 1 //PHONE
	#define JL_CT_ALLP(nintendo, computer, android) android
#else //3DS/WiiU
	#define JL_CT_ALLP(nintendo, computer, android) nintendo
#endif

#define JL_CT_MAXX JL_CT_ALLP(\
		JL_CT_NINT_MAXX,\
		JL_CT_COMP_MAXX,\
		JL_CT_ANDR_MAXX)

/*
 * When this function is called, how many input modes your program is going to
 * have is determined by the number of modes you have created.
 * Allocating space for each input mode is done by using "JL_CT_setm()"
*/
void jl_ct_mode_init(jl_t *pusr);

/*
 * Allocate space for how many controls you want.
 * The Maximum is 255.  You won't need that much ever!!!
 * You can set controls with:
 *
 *    jl_ct_addi()
 *        This adds a simple input button
 *    jl_ct_addr()
 *        This adds a directional button (A Joystick/Arrow Keys/WASD etc.)
 *        This function has 4 directions: UP/DOWN/LEFT/RIGHT
 *    jl_ct_adds()
 *        This adds a directional button (A Joystick/Arrow Keys/WASD etc.)
 *        This function has infinite directions; it requires 1 function pointer
 *        void OnEvent(int8_t x, int8_t y); this function is called every frame.
 *        x is set to joystick etc.'s x position and y is set to it's y
 *        position.
 *    jl_ct_adde()
 *        This adds a directional button (A Joystick/Arrow Keys/WASD etc.)
 *        This function had 8 directions so it requires 8 function pointers.
 *    jl_ct_addn()
 *        This adds a directional button (A Joystick/Arrow Keys/WASD etc.)
 *        This function has 10 directions that match up to the number pad on
 *        The computer. On other platforms it depends on how you set it up
 *    jl_ct_adda()
 *        This adds a directional button but, it can't be a joystick or d-pad.
 *        For non-PC/Mac stuff it has to use the ABXY keys.  This is commonly
 *        Used for attack combos in games (That's really it's only use I think)
 *        There is 15 different combinations that you can make.
*/
void jl_ct_mode_setm(jl_t *pusr, uint8_t loop, uint8_t mode,
	uint8_t controlCount);
/*
 * Set input mode and update controls on the screen.  Call this after you set
 * all of the controls
*/
void jl_ct_updm(uint8_t mode);

/*
 * This adds a simple input button.
 * Control with Id usrevent is set to event libevent.
 * When libevent happens, event[usrevent].fn is called.
 * Ex:
 *	#define USR_DEFINED_EVENT1 0
 *	#define USR_DEFINED_EVENTMAX 1
 *	JL_CT_addi(
 *		JL_CT_ALLP(JL_CT_NINT_CPUP, JL_CT_COMP_AROU,
 *			JL_CT_ANDR_TFUP), //Returns event for platform
 *		USR_DEFINED_EVENT1, onEvent);
*/
void jl_ct_mode_addi(jl_t *pusr, uint8_t libevent, uint8_t usrevent,
	void (*fn)(jl_t *pusr, float x, float y));
/*
 * This adds a directional button (A Joystick/Arrow Keys/WASD etc.)
 * This function has 4 directions: UP/DOWN/LEFT/RIGHT
 * it requires you to give it 4 function pointers:
 * void up(uint8_t p); void down(uint8_t p);
 * void left(uint8_t p); void right(uint8_t p);
*/
void jl_ct_addr(uint8_t controlNum,
	void (*up)(uint8_t p), void (*down)(uint8_t p),
	void (*right)(uint8_t p), void (*left)(uint8_t p)
	);
void jl_ct_adds(void);
void jl_ct_adde(void);
void jl_ct_addn(void);
void jl_ct_adda(void);

float jl_ct_gmousex(jl_t *pusr);
float jl_ct_gmousey(jl_t *pusr);

/*
 * Returns 0 if key isn't pressed
 * Returns 1 if key is just pressed
 * Returns 2 if key is held down
 * Returns 3 if key is released
*/
uint8_t jl_ct_key_pressed(jl_t *pusr, uint8_t key);
