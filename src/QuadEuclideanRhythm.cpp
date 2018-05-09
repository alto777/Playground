#include <string.h>
#include "Playground.hpp"
#include "dsp/digital.hpp"
#define NUM_RULERS 10
#define MAX_DIVISIONS 6
#define TRACK_COUNT 4
#define MAX_STEPS 18

struct QuadEuclideanRhythm : Module {
	enum ParamIds {
		STEPS_1_PARAM,
		DIVISIONS_1_PARAM,
		OFFSET_1_PARAM,
		PAD_1_PARAM,
		ACCENTS_1_PARAM,
		ACCENT_ROTATE_1_PARAM,
		STEPS_2_PARAM,
		DIVISIONS_2_PARAM,
		OFFSET_2_PARAM,
		PAD_2_PARAM,
		ACCENTS_2_PARAM,
		ACCENT_ROTATE_2_PARAM,
		STEPS_3_PARAM,
		DIVISIONS_3_PARAM,
		OFFSET_3_PARAM,
		PAD_3_PARAM,
		ACCENTS_3_PARAM,
		ACCENT_ROTATE_3_PARAM,
		STEPS_4_PARAM,
		DIVISIONS_4_PARAM,
		OFFSET_4_PARAM,
		PAD_4_PARAM,
		ACCENTS_4_PARAM,
		ACCENT_ROTATE_4_PARAM,
		CHAIN_MODE_PARAM,	
		CONSTANT_TIME_MODE_PARAM,	
		NUM_PARAMS
	};
	enum InputIds {
		STEPS_1_INPUT,
		DIVISIONS_1_INPUT,
		OFFSET_1_INPUT,
		PAD_1_INPUT,
		ACCENTS_1_INPUT,
		ACCENT_ROTATE_1_INPUT,
		START_1_INPUT,
		STEPS_2_INPUT,
		DIVISIONS_2_INPUT,
		OFFSET_2_INPUT,
		PAD_2_INPUT,
		ACCENTS_2_INPUT,
		ACCENT_ROTATE_2_INPUT,
		START_2_INPUT,
		STEPS_3_INPUT,
		DIVISIONS_3_INPUT,
		OFFSET_3_INPUT,
		PAD_3_INPUT,
		ACCENTS_3_INPUT,
		ACCENT_ROTATE_3_INPUT,
		START_3_INPUT,
		STEPS_4_INPUT,
		DIVISIONS_4_INPUT,
		OFFSET_4_INPUT,
		PAD_4_INPUT,
		ACCENTS_4_INPUT,
		ACCENT_ROTATE_4_INPUT,
		START_4_INPUT,
		CLOCK_INPUT,
		RESET_INPUT,
		MUTE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		ACCENT_OUTPUT_1,
		EOC_OUTPUT_1,
		OUTPUT_2,
		ACCENT_OUTPUT_2,
		EOC_OUTPUT_2,
		OUTPUT_3,
		ACCENT_OUTPUT_3,
		EOC_OUTPUT_3,
		OUTPUT_4,
		ACCENT_OUTPUT_4,
		EOC_OUTPUT_4,
		NUM_OUTPUTS
	};
	enum LightIds {
		CHAIN_MODE_NONE_LIGHT,
		CHAIN_MODE_BOSS_LIGHT,
		CHAIN_MODE_EMPLOYEE_LIGHT,
		MUTED_LIGHT,
		CONSTANT_TIME_LIGHT,
		NUM_LIGHTS
	};
	enum ChainModes {
		CHAIN_MODE_NONE,
		CHAIN_MODE_BOSS,
		CHAIN_MODE_EMPLOYEE
	};

	bool beatMatrix[TRACK_COUNT][MAX_STEPS];
	bool accentMatrix[TRACK_COUNT][MAX_STEPS];
	int beatIndex[TRACK_COUNT];
	int stepsCount[TRACK_COUNT];
	float stepDuration[TRACK_COUNT];
	float lastStepTime[TRACK_COUNT];
	float maxStepCount;

	bool running[TRACK_COUNT];
	int chainMode;
	bool initialized = false;
	bool muted = false;
	bool constantTime = false;

	float time = 0.0;
	float duration = 0.0;
	bool secondClockReceived = false;

	SchmittTrigger clockTrigger,resetTrigger,chainModeTrigger,constantTimeTrigger,muteTrigger,startTrigger[TRACK_COUNT];
	PulseGenerator beatPulse[TRACK_COUNT],accentPulse[TRACK_COUNT],eocPulse[TRACK_COUNT];



	QuadEuclideanRhythm() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS,NUM_LIGHTS) {
		for(unsigned i = 0; i < TRACK_COUNT; i++) {
			beatIndex[i] = 0;
			stepsCount[i] = MAX_STEPS;
			lastStepTime[i] = 0.0;
			stepDuration[i] = 0.0;
			running[i] = true;
			for(unsigned j = 0; j < MAX_STEPS; j++) {
				beatMatrix[i][j] = false;
				accentMatrix[i][j] = false;
			}
		}		
	}
	void step() override;


	json_t *toJson() override {
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "constantTime", json_integer((bool) constantTime));
		json_object_set_new(rootJ, "chainMode", json_integer((int) chainMode));
		json_object_set_new(rootJ, "muted", json_integer((bool) muted));
		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		json_t *ctJ = json_object_get(rootJ, "constantTime");
		if (ctJ)
			constantTime = json_integer_value(ctJ);

		json_t *cmJ = json_object_get(rootJ, "chainMode");
		if (cmJ)
			chainMode = json_integer_value(cmJ);

		json_t *mutedJ = json_object_get(rootJ, "muted");
		if (mutedJ)
			muted = json_integer_value(mutedJ);
	}

	void setRunningState() {
		for(int trackNumber=0;trackNumber<4;trackNumber++)
		{
			if(chainMode == CHAIN_MODE_EMPLOYEE && inputs[(trackNumber * 7) + 6].active) { //START Input needs to be active
				running[trackNumber] = false;
			}
			else {
				running[trackNumber] = true;
			}
		}
	}

	void advanceBeat(int trackNumber) {
		beatIndex[trackNumber]++;
		lastStepTime[trackNumber] = 0.0;					
					
		//End of Cycle
		if(beatIndex[trackNumber] >= stepsCount[trackNumber]) {
			beatIndex[trackNumber] = 0;
			eocPulse[trackNumber].trigger(1e-3);
			//If in a chain mode, stop running until start trigger received
			if(chainMode != CHAIN_MODE_NONE && inputs[(trackNumber * 7) + 6].active) { //START Input needs to be active
				running[trackNumber] = false;
			}
		}
	}
	
/* begin table hack */

//static const 
unsigned char tableOffset[17] = {
	0, 1, 3, 6,
	10, 15, 21, 28,
	36, 45, 55, 66,
	78, 91, 105, 120,
	136,
};

//static const
unsigned int euclideanBits[153] = {
	0x0000,	/* E( 0,  0) = 0x0000   */

	0x0000,	/* E( 0,  1) = 0x0000  . */
	0x0001,	/* E( 1,  1) = 0x0001  X */

	0x0000,	/* E( 0,  2) = 0x0000  .. */
	0x0002,	/* E( 1,  2) = 0x0002  .X */
	0x0003,	/* E( 2,  2) = 0x0003  XX */

	0x0000,	/* E( 0,  3) = 0x0000  ... */
	0x0004,	/* E( 1,  3) = 0x0004  ..X */
	0x0003,	/* E( 2,  3) = 0x0003  XX. */
	0x0007,	/* E( 3,  3) = 0x0007  XXX */

	0x0000,	/* E( 0,  4) = 0x0000  .... */
	0x0008,	/* E( 1,  4) = 0x0008  ...X */
	0x000a,	/* E( 2,  4) = 0x000a  .X.X */
	0x0007,	/* E( 3,  4) = 0x0007  XXX. */
	0x000f,	/* E( 4,  4) = 0x000f  XXXX */

	0x0000,	/* E( 0,  5) = 0x0000  ..... */
	0x0010,	/* E( 1,  5) = 0x0010  ....X */
	0x000a,	/* E( 2,  5) = 0x000a  .X.X. */
	0x0015,	/* E( 3,  5) = 0x0015  X.X.X */
	0x000f,	/* E( 4,  5) = 0x000f  XXXX. */
	0x001f,	/* E( 5,  5) = 0x001f  XXXXX */

	0x0000,	/* E( 0,  6) = 0x0000  ...... */
	0x0020,	/* E( 1,  6) = 0x0020  .....X */
	0x0024,	/* E( 2,  6) = 0x0024  ..X..X */
	0x002a,	/* E( 3,  6) = 0x002a  .X.X.X */
	0x001b,	/* E( 4,  6) = 0x001b  XX.XX. */
	0x001f,	/* E( 5,  6) = 0x001f  XXXXX. */
	0x003f,	/* E( 6,  6) = 0x003f  XXXXXX */

	0x0000,	/* E( 0,  7) = 0x0000  ....... */
	0x0040,	/* E( 1,  7) = 0x0040  ......X */
	0x0024,	/* E( 2,  7) = 0x0024  ..X..X. */
	0x002a,	/* E( 3,  7) = 0x002a  .X.X.X. */
	0x0055,	/* E( 4,  7) = 0x0055  X.X.X.X */
	0x005b,	/* E( 5,  7) = 0x005b  XX.XX.X */
	0x003f,	/* E( 6,  7) = 0x003f  XXXXXX. */
	0x007f,	/* E( 7,  7) = 0x007f  XXXXXXX */

	0x0000,	/* E( 0,  8) = 0x0000  ........ */
	0x0080,	/* E( 1,  8) = 0x0080  .......X */
	0x0088,	/* E( 2,  8) = 0x0088  ...X...X */
	0x0092,	/* E( 3,  8) = 0x0092  .X..X..X */
	0x00aa,	/* E( 4,  8) = 0x00aa  .X.X.X.X */
	0x006d,	/* E( 5,  8) = 0x006d  X.XX.XX. */
	0x0077,	/* E( 6,  8) = 0x0077  XXX.XXX. */
	0x007f,	/* E( 7,  8) = 0x007f  XXXXXXX. */
	0x00ff,	/* E( 8,  8) = 0x00ff  XXXXXXXX */

	0x0000,	/* E( 0,  9) = 0x0000  ......... */
	0x0100,	/* E( 1,  9) = 0x0100  ........X */
	0x0088,	/* E( 2,  9) = 0x0088  ...X...X. */
	0x0124,	/* E( 3,  9) = 0x0124  ..X..X..X */
	0x00aa,	/* E( 4,  9) = 0x00aa  .X.X.X.X. */
	0x0155,	/* E( 5,  9) = 0x0155  X.X.X.X.X */
	0x00db,	/* E( 6,  9) = 0x00db  XX.XX.XX. */
	0x0177,	/* E( 7,  9) = 0x0177  XXX.XXX.X */
	0x00ff,	/* E( 8,  9) = 0x00ff  XXXXXXXX. */
	0x01ff,	/* E( 9,  9) = 0x01ff  XXXXXXXXX */

	0x0000,	/* E( 0, 10) = 0x0000  .......... */
	0x0200,	/* E( 1, 10) = 0x0200  .........X */
	0x0210,	/* E( 2, 10) = 0x0210  ....X....X */
	0x0124,	/* E( 3, 10) = 0x0124  ..X..X..X. */
	0x014a,	/* E( 4, 10) = 0x014a  .X.X..X.X. */
	0x02aa,	/* E( 5, 10) = 0x02aa  .X.X.X.X.X */
	0x02b5,	/* E( 6, 10) = 0x02b5  X.X.XX.X.X */
	0x02db,	/* E( 7, 10) = 0x02db  XX.XX.XX.X */
	0x01ef,	/* E( 8, 10) = 0x01ef  XXXX.XXXX. */
	0x01ff,	/* E( 9, 10) = 0x01ff  XXXXXXXXX. */
	0x03ff,	/* E(10, 10) = 0x03ff  XXXXXXXXXX */

	0x0000,	/* E( 0, 11) = 0x0000  ........... */
	0x0400,	/* E( 1, 11) = 0x0400  ..........X */
	0x0210,	/* E( 2, 11) = 0x0210  ....X....X. */
	0x0444,	/* E( 3, 11) = 0x0444  ..X...X...X */
	0x0492,	/* E( 4, 11) = 0x0492  .X..X..X..X */
	0x02aa,	/* E( 5, 11) = 0x02aa  .X.X.X.X.X. */
	0x0555,	/* E( 6, 11) = 0x0555  X.X.X.X.X.X */
	0x036d,	/* E( 7, 11) = 0x036d  X.XX.XX.XX. */
	0x03bb,	/* E( 8, 11) = 0x03bb  XX.XXX.XXX. */
	0x05ef,	/* E( 9, 11) = 0x05ef  XXXX.XXXX.X */
	0x03ff,	/* E(10, 11) = 0x03ff  XXXXXXXXXX. */
	0x07ff,	/* E(11, 11) = 0x07ff  XXXXXXXXXXX */

	0x0000,	/* E( 0, 12) = 0x0000  ............ */
	0x0800,	/* E( 1, 12) = 0x0800  ...........X */
	0x0820,	/* E( 2, 12) = 0x0820  .....X.....X */
	0x0888,	/* E( 3, 12) = 0x0888  ...X...X...X */
	0x0924,	/* E( 4, 12) = 0x0924  ..X..X..X..X */
	0x094a,	/* E( 5, 12) = 0x094a  .X.X..X.X..X */
	0x0aaa,	/* E( 6, 12) = 0x0aaa  .X.X.X.X.X.X */
	0x06b5,	/* E( 7, 12) = 0x06b5  X.X.XX.X.XX. */
	0x06db,	/* E( 8, 12) = 0x06db  XX.XX.XX.XX. */
	0x0777,	/* E( 9, 12) = 0x0777  XXX.XXX.XXX. */
	0x07df,	/* E(10, 12) = 0x07df  XXXXX.XXXXX. */
	0x07ff,	/* E(11, 12) = 0x07ff  XXXXXXXXXXX. */
	0x0fff,	/* E(12, 12) = 0x0fff  XXXXXXXXXXXX */

	0x0000,	/* E( 0, 13) = 0x0000  ............. */
	0x1000,	/* E( 1, 13) = 0x1000  ............X */
	0x0820,	/* E( 2, 13) = 0x0820  .....X.....X. */
	0x0888,	/* E( 3, 13) = 0x0888  ...X...X...X. */
	0x0924,	/* E( 4, 13) = 0x0924  ..X..X..X..X. */
	0x0a52,	/* E( 5, 13) = 0x0a52  .X..X.X..X.X. */
	0x0aaa,	/* E( 6, 13) = 0x0aaa  .X.X.X.X.X.X. */
	0x1555,	/* E( 7, 13) = 0x1555  X.X.X.X.X.X.X */
	0x15ad,	/* E( 8, 13) = 0x15ad  X.XX.X.XX.X.X */
	0x16db,	/* E( 9, 13) = 0x16db  XX.XX.XX.XX.X */
	0x1777,	/* E(10, 13) = 0x1777  XXX.XXX.XXX.X */
	0x17df,	/* E(11, 13) = 0x17df  XXXXX.XXXXX.X */
	0x0fff,	/* E(12, 13) = 0x0fff  XXXXXXXXXXXX. */
	0x1fff,	/* E(13, 13) = 0x1fff  XXXXXXXXXXXXX */

	0x0000,	/* E( 0, 14) = 0x0000  .............. */
	0x2000,	/* E( 1, 14) = 0x2000  .............X */
	0x2040,	/* E( 2, 14) = 0x2040  ......X......X */
	0x2108,	/* E( 3, 14) = 0x2108  ...X....X....X */
	0x1224,	/* E( 4, 14) = 0x1224  ..X..X...X..X. */
	0x2492,	/* E( 5, 14) = 0x2492  .X..X..X..X..X */
	0x152a,	/* E( 6, 14) = 0x152a  .X.X.X..X.X.X. */
	0x2aaa,	/* E( 7, 14) = 0x2aaa  .X.X.X.X.X.X.X */
	0x2ad5,	/* E( 8, 14) = 0x2ad5  X.X.X.XX.X.X.X */
	0x1b6d,	/* E( 9, 14) = 0x1b6d  X.XX.XX.XX.XX. */
	0x2ddb,	/* E(10, 14) = 0x2ddb  XX.XX.XXX.XX.X */
	0x1ef7,	/* E(11, 14) = 0x1ef7  XXX.XXXX.XXXX. */
	0x1fbf,	/* E(12, 14) = 0x1fbf  XXXXXX.XXXXXX. */
	0x1fff,	/* E(13, 14) = 0x1fff  XXXXXXXXXXXXX. */
	0x3fff,	/* E(14, 14) = 0x3fff  XXXXXXXXXXXXXX */

	0x0000,	/* E( 0, 15) = 0x0000  ............... */
	0x4000,	/* E( 1, 15) = 0x4000  ..............X */
	0x2040,	/* E( 2, 15) = 0x2040  ......X......X. */
	0x4210,	/* E( 3, 15) = 0x4210  ....X....X....X */
	0x4444,	/* E( 4, 15) = 0x4444  ..X...X...X...X */
	0x4924,	/* E( 5, 15) = 0x4924  ..X..X..X..X..X */
	0x294a,	/* E( 6, 15) = 0x294a  .X.X..X.X..X.X. */
	0x2aaa,	/* E( 7, 15) = 0x2aaa  .X.X.X.X.X.X.X. */
	0x5555,	/* E( 8, 15) = 0x5555  X.X.X.X.X.X.X.X */
	0x56b5,	/* E( 9, 15) = 0x56b5  X.X.XX.X.XX.X.X */
	0x36db,	/* E(10, 15) = 0x36db  XX.XX.XX.XX.XX. */
	0x3bbb,	/* E(11, 15) = 0x3bbb  XX.XXX.XXX.XXX. */
	0x3def,	/* E(12, 15) = 0x3def  XXXX.XXXX.XXXX. */
	0x5fbf,	/* E(13, 15) = 0x5fbf  XXXXXX.XXXXXX.X */
	0x3fff,	/* E(14, 15) = 0x3fff  XXXXXXXXXXXXXX. */
	0x7fff,	/* E(15, 15) = 0x7fff  XXXXXXXXXXXXXXX */

	0x0000,	/* E( 0, 16) = 0x0000  ................ */
	0x8000,	/* E( 1, 16) = 0x8000  ...............X */
	0x8080,	/* E( 2, 16) = 0x8080  .......X.......X */
	0x4210,	/* E( 3, 16) = 0x4210  ....X....X....X. */
	0x8888,	/* E( 4, 16) = 0x8888  ...X...X...X...X */
	0x4924,	/* E( 5, 16) = 0x4924  ..X..X..X..X..X. */
	0x9292,	/* E( 6, 16) = 0x9292  .X..X..X.X..X..X */
	0x952a,	/* E( 7, 16) = 0x952a  .X.X.X..X.X.X..X */
	0xaaaa,	/* E( 8, 16) = 0xaaaa  .X.X.X.X.X.X.X.X */
	0x6ad5,	/* E( 9, 16) = 0x6ad5  X.X.X.XX.X.X.XX. */
	0x6d6d,	/* E(10, 16) = 0x6d6d  X.XX.XX.X.XX.XX. */
	0xb6db,	/* E(11, 16) = 0xb6db  XX.XX.XX.XX.XX.X */
	0x7777,	/* E(12, 16) = 0x7777  XXX.XXX.XXX.XXX. */
	0xbdef,	/* E(13, 16) = 0xbdef  XXXX.XXXX.XXXX.X */
	0x7f7f,	/* E(14, 16) = 0x7f7f  XXXXXXX.XXXXXXX. */
	0x7fff,	/* E(15, 16) = 0x7fff  XXXXXXXXXXXXXXX. */
	0xffff,	/* E(16, 16) = 0xffff  XXXXXXXXXXXXXXXX */
};

unsigned int euclideanRhythm(unsigned char pulses, unsigned char slots)
{
	return  euclideanBits[tableOffset[slots] + pulses];
}
/* end table hack */	
	
	// For more advanced Module features, read Rack's engine.hpp header file
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};





void QuadEuclideanRhythm::step() {

	int levelArray[16];
	int accentLevelArray[16];
	int beatLocation[16];

	//Set startup state
	if(!initialized) {
		setRunningState();
		initialized = true;
	}

	// Modes
	if (constantTimeTrigger.process(params[CONSTANT_TIME_MODE_PARAM].value)) {
		constantTime = !constantTime;
		for(int trackNumber=0;trackNumber<4;trackNumber++) {
			beatIndex[trackNumber] = 0;
		}
		setRunningState();
	}
	lights[CONSTANT_TIME_LIGHT].value = constantTime;

	if (chainModeTrigger.process(params[CHAIN_MODE_PARAM].value)) {
		chainMode = (chainMode + 1) % 3;
		setRunningState();
	}
	lights[CHAIN_MODE_NONE_LIGHT].value = chainMode == CHAIN_MODE_NONE ? 1.0 : 0.0;
	lights[CHAIN_MODE_BOSS_LIGHT].value = chainMode == CHAIN_MODE_BOSS ? 1.0 : 0.0;
	lights[CHAIN_MODE_EMPLOYEE_LIGHT].value = chainMode == CHAIN_MODE_EMPLOYEE ? 1.0 : 0.0;

	lights[MUTED_LIGHT].value = muted ? 1.0 : 0.0;

	maxStepCount = 0;

	for(int trackNumber=0;trackNumber<4;trackNumber++) {
		//clear out the matrix and levels
		for(int j=0;j<16;j++)
		{
			beatMatrix[trackNumber][j] = false; 
			accentMatrix[trackNumber][j] = false;
			levelArray[j] = 0;		
			accentLevelArray[j] = 0;	
			beatLocation[j] = 0;
		}

		float stepsCountf = params[trackNumber * 6].value;
		if(inputs[trackNumber * 7].active) {
			stepsCountf += inputs[trackNumber * 7].value;
		}
		stepsCountf = clamp(stepsCountf,0.0f,16.0f);	

		float divisionf = params[(trackNumber * 6) + 1].value;
		if(inputs[(trackNumber * 7) + 1].active) {
			divisionf += inputs[(trackNumber * 7) + 1].value;
		}		
		divisionf = clamp(divisionf,0.0f,stepsCountf);

		float offsetf = params[(trackNumber * 6) + 2].value;
		if(inputs[(trackNumber * 7) + 2].active) {
			offsetf += inputs[(trackNumber * 7) + 2].value;
		}	
		offsetf = clamp(offsetf,0.0f,15.0f);

		float padf = params[trackNumber * 6 + 3].value;
		if(inputs[trackNumber * 7 + 3].active) {
			padf += inputs[trackNumber * 7 + 3].value;
		}
		padf = clamp(padf,0.0f,stepsCountf - divisionf);


		//Use this to reduce range of accent params/inputs so the range of motion of knob/modulation is more useful.
		float divisionScale = 1;
		if(stepsCountf > 0) {
			divisionScale = divisionf / stepsCountf;
		}		

		float accentDivisionf = params[(trackNumber * 6) + 4].value * divisionScale;
		if(inputs[(trackNumber * 7) + 4].active) {
			accentDivisionf += inputs[(trackNumber * 7) + 4].value * divisionScale;
		}
		accentDivisionf = clamp(accentDivisionf,0.0f,divisionf);

		float accentRotationf = params[(trackNumber * 6) + 5].value * divisionScale;
		if(inputs[(trackNumber * 7) + 5].active) {
			accentRotationf += inputs[(trackNumber * 7) + 5].value * divisionScale;
		}
		if(divisionf > 0) {
			accentRotationf = clamp(accentRotationf,0.0f,divisionf-1);			
		} else {
			accentRotationf = 0;
		}

		if(stepsCountf > maxStepCount)
			maxStepCount = stepsCountf;


		stepsCount[trackNumber] = int(stepsCountf);

		int division = int(divisionf);
		int offset = int(offsetf);		
		int pad = int(padf);
		int accentDivision = int(accentDivisionf);
		int accentRotation = int(accentRotationf);

		if(stepsCount[trackNumber] > 0) {
			//Calculate Beats


/*
we need to come out of this blocked off section with just 

beatMatrix[trackNumber] - padded offset (rotated) euclidean rhythm of length stepsCount[trackNumber]
beatLocation[beatIndexed] - where the beets ended up



			int level = 0;
			int restsLeft = std::max(0,stepsCount[trackNumber]-division-pad); // just make sure no negatives
			do {
				levelArray[level] = std::min(restsLeft,division); 
				restsLeft = restsLeft - division;
				level += 1;
			} while (restsLeft > 0 && level < 16);

			int tempIndex =pad;
			int beatIndex = 0;
			for (int j = 0; j < division; j++) {
	            beatMatrix[trackNumber][(tempIndex + offset) % stepsCount[trackNumber]] = true;
	            beatLocation[beatIndex] = (tempIndex + offset) % stepsCount[trackNumber];
	            tempIndex++;
	            beatIndex++;
	            for (int k = 0; k < 16; k++) {
	                if (levelArray[k] > j) {
	                    tempIndex++;
	                }
	            }
	        }	
*/


/* retrieve right-justified Euclidean */

			int nSteps = stepsCount[trackNumber];

/* hmmm this shouldn't happen: */
			if (division > (nSteps - pad)) division = nSteps - pad; /* <- this deosn't fix it #07 */
			if (offset >= nSteps) offset -= nSteps;
/* crush this first - no pad. no offset */
/* now try adding pad. pad just reduces nSteps available for bits */
/* but you must? can? still roll in all nSteps bits later */
/* now add offset to parking place */
			unsigned int theBits = euclideanBits[tableOffset[nSteps - pad] + division];	/* need to check no negatives? */

/* roll it into the beat array and index it */
/* weirdly filled the matrix area backwards? hence nSteps - j - 1 below */
/* #07 oddly - when division is increased it first fills up leaving pad area alone, then eats into the pad? */

			int beatIndex = 0;
			for (int j = 0; j < nSteps; j++, theBits >>= 1) {
				if (theBits & 0x1) {
					int slot = j + offset;
					if (slot >= nSteps) slot -= nSteps;
					beatMatrix[trackNumber][slot] = true;
					beatLocation[beatIndex++] = slot;
				}
			}

/* broke accents tho */

/*
			int level = 0;
			int restsLeft = std::max(0,stepsCount[trackNumber]-division-pad); // just make sure no negatives

	        //Calculate Accents UH OH THIS DOESN'T WORK YET it depended on something from above
	        // I certainly hope can be calculated
	        int tempIndex = pad;		// just so it complies. accentDivs must = 0 now
	        level = 0;
	        restsLeft = std::max(0,division-accentDivision); // just make sure no negatives
	        do {
				accentLevelArray[level] = std::min(restsLeft,accentDivision);
				restsLeft = restsLeft - accentDivision;
				level += 1;
			} while (restsLeft > 0 && level < 16);

			tempIndex =0;
			for (int j = 0; j < accentDivision; j++) {
	            accentMatrix[trackNumber][beatLocation[(tempIndex + accentRotation) % division]] = true;
	            tempIndex++;
	            for (int k = 0; k < 16; k++) {
	                if (accentLevelArray[k] > j) {
	                    tempIndex++;
	                }
	            }
	        }
*/
/* first w/o rotation (offset)... */
/* now add slot for offset */
			theBits = euclideanBits[tableOffset[division] + accentDivision];
			for (int j = 0; j < division; j++, theBits >>= 1) {
				if (theBits & 0x1) {
					int slot = j + accentRotation;
					if (slot >= division) slot -= division;
					accentMatrix[trackNumber][beatLocation[slot]] = true;
				}
			}
        }	
	}

	if(inputs[RESET_INPUT].active) {
		if(resetTrigger.process(inputs[RESET_INPUT].value)) {
			for(int trackNumber=0;trackNumber<4;trackNumber++)
			{
				beatIndex[trackNumber] = 0;
			}
			setRunningState();
		}
	}

	if(inputs[MUTE_INPUT].active) {
		if(muteTrigger.process(inputs[MUTE_INPUT].value)) {
			muted = !muted;
		}
	}

	//See if need to start up
	for(int trackNumber=0;trackNumber < 4;trackNumber++) {
		if(chainMode != CHAIN_MODE_NONE && inputs[(trackNumber * 7) + 6].active && !running[trackNumber]) {
			if(startTrigger[trackNumber].process(inputs[(trackNumber * 7) + 6].value)) {
				running[trackNumber] = true;
			}
		}
	}

	//Advance beat on trigger if not in constant time mode
	float timeAdvance =1.0 / engineGetSampleRate();
    time += timeAdvance;
	if(inputs[CLOCK_INPUT].active) {
		if(clockTrigger.process(inputs[CLOCK_INPUT].value)) {
			if(secondClockReceived) {
				duration = time;
			}
			time = 0;
			secondClockReceived = true;			

			for(int trackNumber=0;trackNumber < TRACK_COUNT;trackNumber++)
			{
				if(running[trackNumber]) {
					if(!constantTime) {
						advanceBeat(trackNumber);
					}					
				}
			}
		}

		bool resyncAll = false;
		//For constant time, don't rely on clock trigger to advance beat, use time
		for(int trackNumber=0;trackNumber < TRACK_COUNT;trackNumber++) {
			if(stepsCount[trackNumber] > 0)
				stepDuration[trackNumber] = duration * maxStepCount / (float)stepsCount[trackNumber];

			if(running[trackNumber]) {
				lastStepTime[trackNumber] +=timeAdvance;
				if(constantTime && stepDuration[trackNumber] > 0.0 && lastStepTime[trackNumber] >= stepDuration[trackNumber]) {
					advanceBeat(trackNumber);
					if(stepsCount[trackNumber] >= (int)maxStepCount && beatIndex[trackNumber] == 0) {
						resyncAll = true;
					}
				}					
			}
		}
		if(resyncAll) {
			for(int trackNumber=0;trackNumber < TRACK_COUNT;trackNumber++) {
				lastStepTime[trackNumber] = 0;
				beatIndex[trackNumber] = 0;
			}
		}
	}


	// Set output to current state
	for(int trackNumber=0;trackNumber<TRACK_COUNT;trackNumber++) {
		float outputValue = (lastStepTime[trackNumber] < stepDuration[trackNumber] / 2) ? 10.0 : 0.0;
		//Send out beat
		if(beatMatrix[trackNumber][beatIndex[trackNumber]] == true && running[trackNumber] && !muted) {
			outputs[trackNumber * 3].value = outputValue;
		} else {
			outputs[trackNumber * 3].value = 0.0;	
		}
		//send out accent
		if(accentMatrix[trackNumber][beatIndex[trackNumber]] == true && running[trackNumber] && !muted) {
			outputs[trackNumber * 3 + 1].value = outputValue;	
		} else {
			outputs[trackNumber * 3 + 1].value = 0.0;	
		}
		//Send out End of Cycle
		outputs[(trackNumber * 3) + 2].value = eocPulse[trackNumber].process(1.0 / engineGetSampleRate()) ? 10.0 : 0;	
	}
}

struct QERBeatDisplay : TransparentWidget {
	QuadEuclideanRhythm *module;
	int frame = 0;
	std::shared_ptr<Font> font;

	QERBeatDisplay() {
		font = Font::load(assetPlugin(plugin, "res/fonts/Sudo.ttf"));
	}

	void drawBox(NVGcontext *vg, float stepNumber, float trackNumber,bool isBeat,bool isAccent,bool isCurrent) {
		
		//nvgSave(vg);
		//Rect b = Rect(Vec(0, 0), box.size);
		//nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
		nvgBeginPath(vg);
		
		float boxX = stepNumber * 22.5;
		float boxY = trackNumber * 22.5;

		float opacity = 0x80; // Testing using opacity for accents

		if(isAccent) {
			opacity = 0xff;
		}


		NVGcolor strokeColor = nvgRGBA(0xef, 0xe0, 0, 0xff);
		NVGcolor fillColor = nvgRGBA(0xef,0xe0,0,opacity);
		if(isCurrent)
		{
			strokeColor = nvgRGBA(0x2f, 0xf0, 0, 0xff);
			fillColor = nvgRGBA(0x2f,0xf0,0,opacity);			
		}

		nvgStrokeColor(vg, strokeColor);
		nvgStrokeWidth(vg, 1.0);
		nvgRect(vg,boxX,boxY,21,21.0);
		if(isBeat) {
			nvgFillColor(vg, fillColor);
			nvgFill(vg);
		}
		nvgStroke(vg);
	}

	void draw(NVGcontext *vg) override {

		for(int trackNumber = 0;trackNumber < 4;trackNumber++) {
			for(int stepNumber = 0;stepNumber < module->stepsCount[trackNumber];stepNumber++) {				
				bool isBeat = module->beatMatrix[trackNumber][stepNumber];
				bool isAccent = module->accentMatrix[trackNumber][stepNumber];
				bool isCurrent = module->beatIndex[trackNumber] == stepNumber && module->running[trackNumber];				
				drawBox(vg, float(stepNumber), float(trackNumber),isBeat,isAccent,isCurrent);
			}

		}
	}
};


struct QuadEuclideanRhythmWidget : ModuleWidget {
	QuadEuclideanRhythmWidget(QuadEuclideanRhythm *module);
};

QuadEuclideanRhythmWidget::QuadEuclideanRhythmWidget(QuadEuclideanRhythm *module) : ModuleWidget(module) {
	box.size = Vec(15*26, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/QuadEuclideanRhythm.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH - 12, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH + 12, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH - 12, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH + 12, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


	{
		QERBeatDisplay *display = new QERBeatDisplay();
		display->module = module;
		display->box.pos = Vec(16, 34);
		display->box.size = Vec(box.size.x-30, 135);
		addChild(display);
	}


	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(22, 138), module, QuadEuclideanRhythm::STEPS_1_PARAM, 0.0, 16.2, 16.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(61, 138), module, QuadEuclideanRhythm::DIVISIONS_1_PARAM, 1.0, 16.2, 2.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(100, 138), module, QuadEuclideanRhythm::OFFSET_1_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(139, 138), module, QuadEuclideanRhythm::PAD_1_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(178, 138), module, QuadEuclideanRhythm::ACCENTS_1_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(217, 138), module, QuadEuclideanRhythm::ACCENT_ROTATE_1_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(22, 195), module, QuadEuclideanRhythm::STEPS_2_PARAM, 0.0, 16.0, 16.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(61, 195), module, QuadEuclideanRhythm::DIVISIONS_2_PARAM, 1.0, 16.2, 2.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(100, 195), module, QuadEuclideanRhythm::OFFSET_2_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(139, 195), module, QuadEuclideanRhythm::PAD_2_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(178, 195), module, QuadEuclideanRhythm::ACCENTS_2_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(217, 195), module, QuadEuclideanRhythm::ACCENT_ROTATE_2_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(22, 252), module, QuadEuclideanRhythm::STEPS_3_PARAM, 0.0, 16.2, 16.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(61, 252), module, QuadEuclideanRhythm::DIVISIONS_3_PARAM, 1.0, 16.2, 2.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(100, 252), module, QuadEuclideanRhythm::OFFSET_3_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(139, 252), module, QuadEuclideanRhythm::PAD_3_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(178, 252), module, QuadEuclideanRhythm::ACCENTS_3_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(217, 252), module, QuadEuclideanRhythm::ACCENT_ROTATE_3_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(22, 309), module, QuadEuclideanRhythm::STEPS_4_PARAM, 0.0, 16.2, 16.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(61, 309), module, QuadEuclideanRhythm::DIVISIONS_4_PARAM, 1.0, 16.2, 2.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(100, 309), module, QuadEuclideanRhythm::OFFSET_4_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(139, 309), module, QuadEuclideanRhythm::PAD_4_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(178, 309), module, QuadEuclideanRhythm::ACCENTS_4_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(217, 309), module, QuadEuclideanRhythm::ACCENT_ROTATE_4_PARAM, 0.0, 15.2, 0.0));
	addParam(ParamWidget::create<CKD6>(Vec(250, 285), module, QuadEuclideanRhythm::CHAIN_MODE_PARAM, 0.0, 1.0, 0.0));
	addParam(ParamWidget::create<CKD6>(Vec(335, 285), module, QuadEuclideanRhythm::CONSTANT_TIME_MODE_PARAM, 0.0, 1.0, 0.0));

	addInput(Port::create<PJ301MPort>(Vec(23, 163), Port::INPUT, module, QuadEuclideanRhythm::STEPS_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(62, 163), Port::INPUT, module, QuadEuclideanRhythm::DIVISIONS_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(101, 163), Port::INPUT, module, QuadEuclideanRhythm::OFFSET_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(140, 163), Port::INPUT, module, QuadEuclideanRhythm::PAD_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(179, 163), Port::INPUT, module, QuadEuclideanRhythm::ACCENTS_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(218, 163), Port::INPUT, module, QuadEuclideanRhythm::ACCENT_ROTATE_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(23, 220), Port::INPUT, module, QuadEuclideanRhythm::STEPS_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(62, 220), Port::INPUT, module, QuadEuclideanRhythm::DIVISIONS_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(101, 220), Port::INPUT, module, QuadEuclideanRhythm::OFFSET_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(140, 220), Port::INPUT, module, QuadEuclideanRhythm::PAD_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(179, 220), Port::INPUT, module, QuadEuclideanRhythm::ACCENTS_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(218, 220), Port::INPUT, module, QuadEuclideanRhythm::ACCENT_ROTATE_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(23, 277), Port::INPUT, module, QuadEuclideanRhythm::STEPS_3_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(62, 277), Port::INPUT, module, QuadEuclideanRhythm::DIVISIONS_3_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(101, 277), Port::INPUT, module, QuadEuclideanRhythm::OFFSET_3_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(140, 277), Port::INPUT, module, QuadEuclideanRhythm::PAD_3_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(179, 277), Port::INPUT, module, QuadEuclideanRhythm::ACCENTS_3_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(218, 277), Port::INPUT, module, QuadEuclideanRhythm::ACCENT_ROTATE_3_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(23, 334), Port::INPUT, module, QuadEuclideanRhythm::STEPS_4_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(62, 334), Port::INPUT, module, QuadEuclideanRhythm::DIVISIONS_4_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(101, 334), Port::INPUT, module, QuadEuclideanRhythm::OFFSET_4_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(140, 334), Port::INPUT, module, QuadEuclideanRhythm::PAD_4_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(179, 334), Port::INPUT, module, QuadEuclideanRhythm::ACCENTS_4_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(218, 334), Port::INPUT, module, QuadEuclideanRhythm::ACCENT_ROTATE_4_INPUT));

	addInput(Port::create<PJ301MPort>(Vec(262, 343), Port::INPUT, module, QuadEuclideanRhythm::CLOCK_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(302, 343), Port::INPUT, module, QuadEuclideanRhythm::RESET_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(335, 343), Port::INPUT, module, QuadEuclideanRhythm::MUTE_INPUT));

	addInput(Port::create<PJ301MPort>(Vec(322, 145), Port::INPUT, module, QuadEuclideanRhythm::START_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(322, 175), Port::INPUT, module, QuadEuclideanRhythm::START_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(322, 205), Port::INPUT, module, QuadEuclideanRhythm::START_3_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(322, 235), Port::INPUT, module, QuadEuclideanRhythm::START_4_INPUT));


	addOutput(Port::create<PJ301MPort>(Vec(255, 145), Port::OUTPUT, module, QuadEuclideanRhythm::OUTPUT_1));
	addOutput(Port::create<PJ301MPort>(Vec(286, 145), Port::OUTPUT, module, QuadEuclideanRhythm::ACCENT_OUTPUT_1));
	addOutput(Port::create<PJ301MPort>(Vec(354, 145), Port::OUTPUT, module, QuadEuclideanRhythm::EOC_OUTPUT_1));
	addOutput(Port::create<PJ301MPort>(Vec(256, 175), Port::OUTPUT, module, QuadEuclideanRhythm::OUTPUT_2));
	addOutput(Port::create<PJ301MPort>(Vec(286, 175), Port::OUTPUT, module, QuadEuclideanRhythm::ACCENT_OUTPUT_2));
	addOutput(Port::create<PJ301MPort>(Vec(354, 175), Port::OUTPUT, module, QuadEuclideanRhythm::EOC_OUTPUT_2));
	addOutput(Port::create<PJ301MPort>(Vec(256, 205), Port::OUTPUT, module, QuadEuclideanRhythm::OUTPUT_3));
	addOutput(Port::create<PJ301MPort>(Vec(286, 205), Port::OUTPUT, module, QuadEuclideanRhythm::ACCENT_OUTPUT_3));
	addOutput(Port::create<PJ301MPort>(Vec(354, 205), Port::OUTPUT, module, QuadEuclideanRhythm::EOC_OUTPUT_3));
	addOutput(Port::create<PJ301MPort>(Vec(256, 235), Port::OUTPUT, module, QuadEuclideanRhythm::OUTPUT_4));
	addOutput(Port::create<PJ301MPort>(Vec(286, 235), Port::OUTPUT, module, QuadEuclideanRhythm::ACCENT_OUTPUT_4));
	addOutput(Port::create<PJ301MPort>(Vec(354, 235), Port::OUTPUT, module, QuadEuclideanRhythm::EOC_OUTPUT_4));
	
	addChild(ModuleLightWidget::create<SmallLight<BlueLight>>(Vec(282, 285), module, QuadEuclideanRhythm::CHAIN_MODE_NONE_LIGHT));
	addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(Vec(282, 300), module, QuadEuclideanRhythm::CHAIN_MODE_BOSS_LIGHT));
	addChild(ModuleLightWidget::create<SmallLight<RedLight>>(Vec(282, 315), module, QuadEuclideanRhythm::CHAIN_MODE_EMPLOYEE_LIGHT));

	addChild(ModuleLightWidget::create<LargeLight<RedLight>>(Vec(363, 347), module, QuadEuclideanRhythm::MUTED_LIGHT));
	addChild(ModuleLightWidget::create<SmallLight<BlueLight>>(Vec(370, 295), module, QuadEuclideanRhythm::CONSTANT_TIME_LIGHT));
	
}

Model *modelQuadEuclideanRhythm = Model::create<QuadEuclideanRhythm, QuadEuclideanRhythmWidget>("Playground", "QuadEuclideanRhythm", "Tabled Euclidean Rhythm", SEQUENCER_TAG);
