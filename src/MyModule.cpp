#include "Playground.hpp"
#include "dsp/digital.hpp" 

struct MyModule : Module {
	enum ParamIds {
		PITCH_PARAM,

/* slide switch 2 position */		
		SS2_PARAM,

/* slide switch 3 position */		
		SS3_PARAM,

/* toggle 2 state switch */
		TOG2_PARAM,

/* toggle 3 state switch */
		TOG3_PARAM,

/* illuminatable pushbutton */
		LED_BUTTON_PARAM,

/* some Befaco and modifications of */
		SLIDEPOT_PARAM,
		STEP12_PARAM,
		MYKNOB_PARAM,
				
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		
/* slide switch 2 position indicators */
		ENUMS(SS2_LAMPS, 2),	

/* a lamp for the three-way slide switch */
		SS3_LAMP,
		
/* a lamp for the two-way toggle switch */
		TOG2_LAMP,
		
/* a lamp for the three-way slide switch */
		TOG3_LAMP,
		
/* illuminatable pushbutton LAMP */
		LED_BUTTON_LAMP,
				
		NUM_LIGHTS
	};

	
/* pushbutton lamp */
	SchmittTrigger pushButtonTrigger;
	bool pushButtonState = true;

/* hello world global */
	int hwCounter;
	int step12 = 0;

	float phase = 0.0;
	float blinkPhase = 0.0;

	MyModule() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void MyModule::step() {
	// Implement a simple sine oscillator
	float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input
	float pitch = params[PITCH_PARAM].value;
	pitch += params[STEP12_PARAM].value / 12.0;
	pitch += params[SLIDEPOT_PARAM].value / 12.0;
	
	pitch += inputs[PITCH_INPUT].value;
	pitch += inputs[PITCH_INPUT].value;

	pitch = clampf(pitch, -4.0, 4.0);
	// The default pitch is C4
	float freq = 261.626 * powf(2.0, pitch);
	
	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0)
		phase -= 1.0;

	// Compute the sine output
	float sine = sinf(2 * M_PI * phase);
	outputs[SINE_OUTPUT].value = 5.0 * sine;

	// Blink light at 1Hz
	blinkPhase += deltaTime;
	if (blinkPhase >= 1.0)
		blinkPhase -= 1.0;
	lights[BLINK_LIGHT].value = (blinkPhase < 0.5) ? 1.0 : 0.0;
	
//	display[0].frame = 1234;
	
/* two position slide switch sets its lamps */
	
	if (params[SS2_PARAM].value <= 0.0f) {
		lights[SS2_LAMPS].value = 0.0;
		lights[SS2_LAMPS + 1].value = 1.0;
	} else {
		lights[SS2_LAMPS].value = 1.0;
		lights[SS2_LAMPS + 1].value = 0.0;
	}

/* pushbutton toggles its lamp

 add SchnittTrigger object and a boolean to track state like 

	SchmittTrigger pushButtonTrigger;
	bool pushButtonState = true;

- should be written to JSON
*/
		if (pushButtonTrigger.process(params[LED_BUTTON_PARAM].value)) {
			pushButtonState ^= 1;
			if (pushButtonState) hwCounter++; /* Hello World! count turn ons */
		}
		
/* and always set the lamp */		
		lights[LED_BUTTON_LAMP].value = pushButtonState ? 0.8 : 0.2;

/* three-way slide switch */
	lights[SS3_LAMP].value = params[SS3_PARAM].value / 2.0;
		
/* two-way toggle */
	lights[TOG2_LAMP].value = params[TOG2_PARAM].value;;

/* two-way toggle */
	lights[TOG3_LAMP].value = params[TOG3_PARAM].value / 2.0;
	
	step12 = round(params[STEP12_PARAM].value);

}


template <typename BASE>
struct GiantLED : BASE {
	GiantLED() {
		this->box.size = mm2px(Vec(10.0, 10.0));
	}
};


struct myOwnKnob : SVGKnob {
	myOwnKnob() {
		box.size = Vec(40, 40);
		minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
		setSVG(SVG::load(assetPlugin(plugin, "res/myOwnKnob.svg")));
	}
};


/* hello world... */
struct MyModuleDisplay : TransparentWidget {
	MyModule *module;
	int frame = 0;
	std::shared_ptr<Font> font;
	
	MyModuleDisplay() {
		font = Font::load(assetPlugin(plugin, "res/fonts/Sudo.ttf"));
	}

	void draw(NVGcontext *vg) override {
		nvgFontSize(vg, 24);
		nvgFontFaceId(vg, font->handle);
		nvgTextLetterSpacing(vg, -2);
		
		nvgFillColor(vg, nvgRGBA(0x22, 0x11, 0x88, 0x80));
		char text[128];
		snprintf(text, sizeof(text), "Hello World! %d", module->hwCounter + module->step12);
		nvgText(vg, 10, 10, text, NULL);
	}
};
/* ...hello world */

MyModuleWidget::MyModuleWidget() {
	MyModule *module = new MyModule();
	setModule(module);
	box.size = Vec(18 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/MyModule.svg")));
		addChild(panel);
	}

/* hello world... */	
	{
		MyModuleDisplay *display = new MyModuleDisplay();
		display->module = module;
		display->box.pos = Vec(80, 280);
		display->box.size = Vec(box.size.x, 140);
		addChild(display);
	}
/* ...hello world */


/* slide switch 2 position parameter */
	addParam(createParam<CKSS>(Vec(100, 100), module, MyModule::SS2_PARAM, 0.0, 1.0, 1.0));

/* slide switch 2 position indicator LEDs */
	addChild(createLight<MediumLight<GreenLight>>(Vec(102, 85), module, MyModule::SS2_LAMPS));
	addChild(createLight<MediumLight<RedLight>>(Vec(102, 130), module, MyModule::SS2_LAMPS + 1));

/* slide switch 3 position parameter */
	addParam(createParam<CKSSThree>(Vec(150, 100), module, MyModule::SS3_PARAM, 0.0f, 2.0f, 0.0f));
/* and a lamp it controls the brightness of */
	addChild(createLight<LargeLight<BlueLight>>(Vec(149, 80), module, MyModule::SS3_LAMP));

// two state toggle switch
	addParam(createParam<NKK>(Vec(150, 200), module, MyModule::TOG2_PARAM, 0.0, 1.0, 1.0));
/* and a lamp it controls */
	addChild(createLight<LargeLight<GreenLight>>(Vec(157, 180), module, MyModule::TOG2_LAMP));

// three state toggle switch
	addParam(createParam<NKK>(Vec(185, 200), module, MyModule::TOG3_PARAM, 0.0, 2.0, 1.0));
/* and a lamp it controls the brightness of */
//	addChild(createLight<LargeLight<RedLight>>(Vec(192, 180), module, MyModule::TOG3_LAMP));
		addChild(createLight<GiantLED<BlueLight>>(Vec(187, 160), module, MyModule::TOG3_LAMP));

/* illuminatable pushbutton */
	addParam(createParam<LEDButton>(Vec(100, 200 + 125), module, MyModule::LED_BUTTON_PARAM, 0.0, 1.0, 0.0));
	addChild(createLight<MediumLight<GreenLight>>(Vec(104, 204 + 125), module, MyModule::LED_BUTTON_LAMP));
	
	addParam(createParam<BefacoSlidePot>(Vec(12, 116), module, MyModule::SLIDEPOT_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<BefacoBigSnapKnob>(Vec(75, 150), module, MyModule::STEP12_PARAM, 0.0, 12.0, 0.0));

	addParam(createParam<myOwnKnob>(Vec(200, 50), module, MyModule::MYKNOB_PARAM, 0.0, 12.0, 0.0));


	addChild(createScrew<ScrewSilver>(Vec(0, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewBlack>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


	addParam(createParam<Davies1900hBlackKnob>(Vec(28, 87), module, MyModule::PITCH_PARAM, -3.0, 3.0, 0.0));

	addInput(createInput<PJ301MPort>(Vec(33, 186), module, MyModule::PITCH_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(33, 275), module, MyModule::SINE_OUTPUT));

	addChild(createLight<MediumLight<RedLight>>(Vec(41, 59), module, MyModule::BLINK_LIGHT));
}
