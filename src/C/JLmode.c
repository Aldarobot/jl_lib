/*
 * JL_Lib
 * Copyright (c) 2015 Jeron A. Lau 
*/
/** \file
 * JLGRmode.c
 *	Handles things called modes.  An example is: your title screen
 *	of a game and the actual game would be on different modes.
**/
#include "jl_pr.h"

static void _jl_mode_add(jl_t* jl) {
	jvct_t* _jl = jl->_jl;

	// Allocate a new mode.
	_jl->mode.mdes = jl_mem(jl, _jl->mode.mdes,
		(jl->mdec + 1) * sizeof(__sg_mode_t));
	// Set the mode.
	_jl->mode.mdes[jl->mdec].tclp[JL_MODE_INIT] = jl_dont;
	_jl->mode.mdes[jl->mdec].tclp[JL_MODE_LOOP] = jl_dont;
	_jl->mode.mdes[jl->mdec].tclp[JL_MODE_EXIT] = jl_sg_exit;
	// Add to mode count.
	jl->mdec++;
}

/**
 * Set the loop functions for a mode.
 *
 * @param jl: The library context.
 * @param mode: The mode to change the loop functions of.
 * @param wm: Which loop to change.
 *	JL_MODE_INIT: Called when mode is switched in.
 *	JL_MODE_EXIT: Called when "Back Button" Is Pressed.  "Back Button" is:
 *		- 3DS/WiiU: Select
 *		- Android: Back
 *		- Computer: Escape
 *		The default is to quit the program.  If set to something else
 *		then the function will loop forever unless interupted by a
 *		second press of the "Back Button" or unless the mode is changed.
 *	JL_MODE_LOOP: Called repeatedly.
 * @param loop: What to change the loop to.
*/
void jl_mode_set(jl_t* jl, u8_t mode, u8_t wm, jl_fnct loop) {
	jvct_t* _jl = jl->_jl;
	jl_gr_t* jl_gr = jl->jl_gr;

	while(mode >= jl->mdec) _jl_mode_add(jl);
	_jl->mode.mdes[mode].tclp[wm] = loop;
	// Reset things
	if(jl_gr) jl_gr->main.ct.heldDown = 0;
}

/**
 * Temporarily change the mode functions without actually changing the mode.
 * @param jl: The library context.
 * @param wm: the loop to override
 * @param loop: the overriding function 
 */
void jl_mode_override(jl_t* jl, uint8_t wm, jl_fnct loop) {
	jvct_t* _jl = jl->_jl;

	_jl->mode.mode.tclp[wm] = loop;
}

/**
 * Reset any functions overwritten with jl_sg_mode_override().
 * @param jl: The library context.
 */
void jl_mode_reset(jl_t* jl) {
	jvct_t* _jl = jl->_jl;
	int i;

	for(i = 0; i < JL_MODE_LMAX; i++)
		_jl->mode.mode.tclp[i] = _jl->mode.mdes[jl->mode].tclp[i];
}

/**
 * Switch which mode is in use.
 * @param jl: The library context.
 * @param mode: The mode to switch to.
 */
void jl_mode_switch(jl_t* jl, u8_t mode) {
	jvct_t* _jl = jl->_jl;

	// Switch mode
	jl->mode = mode;
	jl->loop = JL_MODE_LOOP;
	// Set the basic functions
	jl_mode_reset(jl);
	// Run user's intialization
	_jl->mode.mode.tclp[JL_MODE_INIT](_jl->jl);
}

// Internal functions

void jl_mode_init__(jl_t* jl) {
	m_u8_t i;
	jvct_t* _jl = jl->_jl;

	// Set up modes:
	jl->loop = JL_MODE_INIT; // Set Default Loop To Initialize
	jl->mode = 0;
	jl->mdec = 0;
	_jl->mode.mdes = NULL;
	_jl_mode_add(jl);
	// Clear User Loops
	for(i = 0; i < JL_MODE_LMAX; i++) _jl->mode.mode.tclp[i] = jl_dont;
}
