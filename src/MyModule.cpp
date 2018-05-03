#include "Playground.hpp"

/* JUST AN ARBITRARY EDIT */
/* arbitrary online edit */

/* just one stop shopping for all I am learning and want to use/figure out */

/* I think I needed this when I introduced a Schmitt Trigger object */
#include "dsp/digital.hpp" 

struct MyModule : Module {
	enum ParamIds {
		PITCH_PARAM,
		LABEL_PARAM,		/* teset JW label */
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
		
		HORIZONTAL_SLIDEPOT_PARAM,

/* IDEA - tiny knob atopa big knob */
		VER_KNOB_PARAM,

/* IDEA LAMP ATOP LAMP both with buttons */
BEZEL_PARAM, INSIDE_PARAM,

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
				
		ENUMS(BICOLOR_LAMP, 2),
		
		ENUMS(TRIADEX_LAMP, 40),
		
		BEZEL_LAMP, INSIDE_LAMP,

		TEST_LAMP,
	
		NUM_LIGHTS
	};

	
/* pushbutton lamp */
	SchmittTrigger pushButtonTrigger;
	bool pushButtonState = true;

/* clock edge detector and Triadex test state */
	bool clockState = 0;	/* we watch the clock for edges, this is state for that */

	int count32 = 0;
	int count3 = 0;
	int count6 = 0;
	unsigned int lfsrBits = 0x0;
	
	bool bezelState = 0, insideState = 0;
	SchmittTrigger bezelTrigger, insideTrigger;

/* hello world global */
	int hwCounter;
	float rotaryEncoderValue = 0;

	float phase = 0.0;
	float blinkPhase = 0.0;

	MyModule() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	void reset() override {
		count32 = 0;
		count3 = 0;
		count6 = 0;
		lfsrBits = 0x0;
	}

	void onCreate() override {
		count32 = 0;
		count3 = 0;
		count6 = 0;
		lfsrBits = 0x0;
	}

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

//pushButtonTrigger.process(params[LED_BUTTON_PARAM].value))
void MyModule::step() {

	if (insideTrigger.process(params[INSIDE_PARAM].value)) {
		insideState ^= 1;
		lights[INSIDE_LAMP].value = insideState ? 10.0f : 0.0f;
	}
	
	if (bezelTrigger.process(params[BEZEL_PARAM].value)) {
		bezelState ^= 1;
		lights[BEZEL_LAMP].value = bezelState ? 10.0f : 0.0f;
	}

	// Implement a simple sine oscillator
	float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input
	float pitch = params[PITCH_PARAM].value;
	pitch += params[STEP12_PARAM].value / 12.0;
	pitch += params[SLIDEPOT_PARAM].value / (12.0 * 20.0);
	pitch += params[VER_KNOB_PARAM].value / 12.0;
	
	rotaryEncoderValue = params[SLIDEPOT_PARAM].value;
	
	pitch += inputs[PITCH_INPUT].value;

	pitch = clamp(pitch, -4.0, 4.0);
	// The default pitch is C4
	float freq = 261.626 * powf(2.0, pitch);
	
	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0)
		phase -= 1.0;

	// Compute the sine output
	float sine = sinf(2 * M_PI * phase);
	outputs[SINE_OUTPUT].value = 5.0 * sine;

/* Blink light at 1Hz
	blinkPhase += deltaTime;
	if (blinkPhase >= 8.0)
		blinkPhase -= 8.0;
	lights[BLINK_LIGHT].value = (blinkPhase < 1.5f) ? 1.0f : 0.0f;
*/	
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
AND A BICOLOR?
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
		
		lights[BICOLOR_LAMP + 0].value =  params[SLIDEPOT_PARAM].value;
		lights[BICOLOR_LAMP + 1].value =  pushButtonState ? 0.0 : 1.0;

/* three-way slide switch */
	lights[SS3_LAMP].value = params[SS3_PARAM].value / 2.0;
		
/* two-way toggle */
	lights[TOG2_LAMP].value = params[TOG2_PARAM].value;

/* two-way toggle */
	lights[TOG3_LAMP].value = params[TOG3_PARAM].value / 2.0;
	
	lights[TEST_LAMP].value = params[TOG3_PARAM].value / 2.0; 

/*	
	for (int i = 0; i < 39; i++) lights[TRIADEX_LAMP + i].value = 1.0;

	int kk = params[SLIDEPOT_PARAM].value;
	if ((kk >= 0) && (kk <= 39))
		lights[TRIADEX_LAMP + kk].value = 0.25;
*/
	blinkPhase += deltaTime;
	if (blinkPhase >= 1.3333f)
		blinkPhase -= 1.3333f;
	lights[BLINK_LIGHT].value = (blinkPhase < 0.6667f) ? 1.0f : 0.0f;
	
	int fakeClock = (blinkPhase < 0.6667f);

//	bool clockState;
	bool risingEdge = 0;
	bool fallingEdge = 0;
	bool stepTime = 0;

	if (fakeClock) {
		if (!clockState) {
			fallingEdge = 1;
			clockState = 1;
		}
	}
	else {
		if (clockState) {
			risingEdge = 1;
			clockState = 0;
		}	
	}
	
	stepTime = risingEdge || fallingEdge;
	

	if (stepTime) {
		count32++; count32 &= 0x1f;
		if (++count3 == 3) {
			count3 = 0;
			count6++; count6 &= 0x3;
		}
		lfsrBits += 0x01010101;		/* just for show */
		if (lfsrBits == 0x3f3f3f3f) lfsrBits = 0;
	}
	

	unsigned int jj;
	int i;
	for (i = 30, jj = 1; i >= 0; i--, jj <<= 1)
		lights[TRIADEX_LAMP + i].value = (lfsrBits & jj) ? 1.0f : 0.0f;
	
	for (i = 32, jj = 1; i >= 31; i--, jj <<= 1)
		lights[TRIADEX_LAMP + i].value = (count6 & jj) ? 1.0f : 0.0f;
	
	for (i = 37, jj = 1; i >= 33; i--, jj <<= 1)
		lights[TRIADEX_LAMP + i].value = (count32 & jj) ? 1.0f : 0.0f;
	
	lights[TRIADEX_LAMP + 39].value = 0.0f;
	lights[TRIADEX_LAMP + 38].value = 1.0f;
	
	
	
//	MyModuleDisplay->display->box.pos = Vec(80, 280);
}


template <typename BASE>
struct GiantLED : BASE {
	GiantLED() {
		this->box.size = mm2px(Vec(10.0, 10.0));
	}
};

template <typename BASE>
struct SmallRectLED : BASE {
	SmallRectLED() {
		this->box.size = mm2px(Vec(4.944f, 8.0f));
	}
};

template <typename BASE>
struct bigLight : BASE {
	bigLight() {
		this->box.size = mm2px(Vec(6.0, 6.0));
	}
};

struct myOwnKnob : SVGKnob {
	myOwnKnob() {
		box.size = Vec(40, 40);
		minAngle = -2.0*M_PI;
		maxAngle = 2.0*M_PI;
		setSVG(SVG::load(assetPlugin(plugin, "res/myOwnKnob.svg")));
//
		shadow->opacity = -1.0;
	}
};

/* JW nice labels and knobs */

struct CenteredLabel : Widget {
	std::string text;
	int fontSize;
	CenteredLabel(int _fontSize = 12) {
		fontSize = _fontSize;
		box.size.y = BND_WIDGET_HEIGHT;
	}
	void draw(NVGcontext *vg) override {
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgFillColor(vg, nvgRGB(25, 150, 252));
		nvgFontSize(vg, fontSize);
		nvgText(vg, box.pos.x, box.pos.y, text.c_str(), NULL);
	}
};

struct MyOwnJWKnob : RoundKnob {
	MyOwnJWKnob() {
		setSVG(SVG::load(assetPlugin(plugin, "res/MyOwnJWKnob.svg")));
	}
	CenteredLabel* linkedLabel = nullptr;
	
	void connectLabel(CenteredLabel* label) {
		linkedLabel = label;
		if (linkedLabel) {
			linkedLabel->text = formatCurrentValue();
		}
	}

	void onChange(EventChange &e) override {
		RoundKnob::onChange(e);
		if (linkedLabel) {

			linkedLabel->text = formatCurrentValue();
		}
	}

	virtual std::string formatCurrentValue() {
		return std::to_string(static_cast<unsigned int>(value));
	}
};

struct AllMyLabels {
	enum labelsEnum {
		KONE,
		KTWO,
		KTHREE,
		KFOUR,
		KFIVE,
		KSXI,
		KSEVEN,
		NUM_LABELS
	};

	const char *theLabels[7] = {
		"a one",
		"anda 2",
		"trois",
		"cat",
		"cinqo"
		"SXI!",
		"SEVEN",
	};

	std::string myLabels(int labelNumber) {
/* huh?
		if ((labelNumber >= 0) && (labelNumber < 7))
			return ("WTF !");
		else
			return " o o p s !";
*/
		switch (labelNumber) {
		case KONE : return("a one (zero)");
		case KTWO : return("anda 2");
		case KTHREE : return("trois");
		case KFOUR : return("cat");
		case KFIVE : return("cinqo");
		case KSXI : return("SXI!");
		case KSEVEN : return("SEVEN");
		case 15 : return ("FIFTEEN");
		case 25 : return ("Twenty Five.");
		case 39 : return ("39th (40 steps).");
		default :
			return ("o o o p s !");		
		}

	}
};

struct MyLabelKnob : MyOwnJWKnob {
	AllMyLabels *labels;
	MyLabelKnob(){
		snap = true;
	}
	std::string formatCurrentValue() override {
		return labels->myLabels(int(value));
	}
};

struct MySnappySlidePot : BefacoSlidePot {
	MySnappySlidePot() {
		snap = true;
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
		snprintf(text, sizeof(text), "ROT! %f", module->rotaryEncoderValue);
		nvgText(vg, 5, 5, text, NULL);
		nvgText(vg, 25, 25, text, NULL);
		nvgText(vg, 25, 125, text, NULL);
	}
};
/* ...hello world */

template <typename BASE>
struct TriadexLight : BASE {
	TriadexLight() {
		this->box.size = Vec(6.0, 6.0);
	}
//	void draw(NVGcontext *vg) override;
};

//template <typename BASE>
//struct rectTriadexLight : BASE {
//	TriadexLight() {
//		this->box.size = Vec(7.0, 4.0);
//	}
////	void draw(NVGcontext *vg) override;
//};

//TriadexLight::draw(NVGcontext *vg) {/* don't yet, just a syntax thing */}

struct my2Switch : SVGSwitch, ToggleSwitch {
	my2Switch() {
		addFrame(SVG::load(assetPlugin(plugin, "res/my2Switch_0.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/my2Switch_1.svg")));
	}
};
/*
struct MyBSlidePot : SVGSlider {
	MyBSlidePot() {
		snap = true;
		Vec margin = Vec(3.5, 3.5);
		maxHandlePos = Vec(-1, -2).plus(margin);
		minHandlePos = Vec(-1, 263).plus(margin);
		setSVGs(SVG::load(assetPlugin(plugin, "res/MyBSlidePot.svg")), SVG::load(assetPlugin(plugin, "res/MyBSlidePotHandle.svg")));
		background->box.pos = margin;
		box.size = background->box.size.plus(margin.mult(2));
	}
};
*/
struct MyBSlidePot : SVGSlider {
	MyBSlidePot() {
		snap = true;
//		Vec margin = Vec(3.5, 3.5);
		maxHandlePos = Vec(-1.65, 0);// .plus(margin);
		minHandlePos = Vec(-1.65, 273);// .plus(margin);
		setSVGs(SVG::load(assetPlugin(plugin, "res/MyBSlidePot.svg")), SVG::load(assetPlugin(plugin, "res/MyBSlidePotHandle.svg")));
//		background->box.pos = margin;
//		box.size = background->box.size.plus(margin.mult(2));
	}
};

struct horizontalSlidePot : SVGSlider {
	horizontalSlidePot() {
		snap = true;
//		Vec margin = Vec(3.5, 3.5);
		maxHandlePos = Vec(0, -1.65);// .plus(margin);
		minHandlePos = Vec(273, -1.65);// .plus(margin);
		setSVGs(SVG::load(assetPlugin(plugin, "res/horizontalSlidePot.svg")), SVG::load(assetPlugin(plugin, "res/horizontalSlidePotHandle.svg")));
//		background->box.pos = margin;
//		box.size = background->box.size.plus(margin.mult(2));
	}
	void onDragMove(EventDragMove &) override;
};

void horizontalSlidePot::onDragMove(EventDragMove &e)
{
	float temp = e.mouseRel.x;
	e.mouseRel.x = e.mouseRel.y;
	e.mouseRel.y = temp;

	SVGSlider::onDragMove(e);
} /* again, just syntax. this kills my fader, natch */

template <typename BASE>
struct x7MMLamp : BASE {
	x7MMLamp() {
		this->box.size = mm2px(Vec(7.0, 7.0));
	}
};

template <typename BASE>
struct x5MMLamp : BASE {
	x5MMLamp() {
		this->box.size = mm2px(Vec(5.0, 5.0));
	}
};

struct x8MMBezel : SVGSwitch, MomentarySwitch {
	x8MMBezel() {
		addFrame(SVG::load(assetPlugin(plugin, "res/x8MMButton.svg")));
	}
};

struct x55MMBezel : SVGSwitch, MomentarySwitch {
	x55MMBezel() {
		addFrame(SVG::load(assetPlugin(plugin, "res/x55MMButton.svg")));
	}
};

/* oops! copied this here to see it, don't need it
...error: reference to 'RedLight' is ambiguous
struct otherRedLight : GrayModuleLightWidget {
	otherRedLight() {
		addBaseColor(nvgRGB(0xff, 0x0, 0x0));
	}
};
*/


struct BLACKModuleLightWidget : ModuleLightWidget {
	BLACKModuleLightWidget() {
		bgColor = nvgRGB(0x0a, 0x0a, 0x0a);
		borderColor = nvgRGBA(0, 0, 0, 0x60);
	}
};


struct rectangularLEDWidget : ModuleLightWidget {
	rectangularLEDWidget() {
		bgColor = nvgRGB(0x40, 0x10, 0x10);
		borderColor = nvgRGBA(0xff, 0xff, 0xff, 0x60);
	}
	
	void drawLight(NVGcontext *) override;
};

void rectangularLEDWidget::drawLight(NVGcontext *vg) {

	nvgBeginPath(vg);
	nvgRect(vg, 0.0, 0.0, box.size.x, box.size.y);

	nvgFillColor(vg, bgColor);
	nvgFill(vg);

	nvgFillColor(vg, color);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRect(vg, 0.5, 0.5, box.size.x - 1.0, box.size.y - 1.0);
	nvgStrokeColor(vg, borderColor);
	nvgStrokeWidth(vg, 1.0);
	nvgStroke(vg);
}

struct whiteRectangularLED : rectangularLEDWidget {
	whiteRectangularLED() {
		addBaseColor(nvgRGB(0xff, 0x80, 0xff));
	}
};

struct myWhiteLight : ModuleLightWidget {
	myWhiteLight() {
		bgColor = nvgRGB(0x40, 0x10, 0x10);
		borderColor = nvgRGBA(0xff, 0xff, 0xff, 0x60);
		addBaseColor(nvgRGB(0xff, 0xff, 0xff));
	}
	
	void drawLight(NVGcontext *) override;
};

void myWhiteLight::drawLight(NVGcontext *vg) {

	nvgBeginPath(vg);
	nvgRect(vg, 0.0, 0.0, box.size.x, box.size.y);

	nvgFillColor(vg, bgColor);
	nvgFill(vg);

	nvgFillColor(vg, color);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRect(vg, 0.5, 0.5, box.size.x - 1.0, box.size.y - 1.0);
	nvgStrokeColor(vg, borderColor);
	nvgStrokeWidth(vg, 1.0);
	nvgStroke(vg);
}

struct myRedLight : BLACKModuleLightWidget {
	myRedLight() {
		addBaseColor(nvgRGB(0xff, 0x0, 0x0));
	}
};

struct MyModuleWidget : ModuleWidget {
	MyModuleWidget(MyModule *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/MyModule.svg")));

/* hello world... */	
	{
		MyModuleDisplay *display = new MyModuleDisplay();
		display->module = module;
		display->box.pos = Vec(25, 25);
//		display->box.size = Vec(box.size.x, 140);
		display->box.size = Vec(140, 140);
		addChild(display);
	}
/* ...hello world */


/* slide switch 2 position parameter */
	addParam(ParamWidget::create<my2Switch>(Vec(100, 100), module, MyModule::SS2_PARAM, 0.0, 1.0, 1.0));

/* slide switch 2 position indicator LEDs */
	addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(102, 85), module, MyModule::SS2_LAMPS));
	addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(102, 130), module, MyModule::SS2_LAMPS + 1));

/* slide switch 3 position parameter */
	addParam(ParamWidget::create<CKSSThree>(Vec(150, 100), module, MyModule::SS3_PARAM, 0.0f, 2.0f, 0.0f));
/* and a lamp it controls the brightness of */
	addChild(ModuleLightWidget::create<LargeLight<BlueLight>>(Vec(149, 80), module, MyModule::SS3_LAMP));

// two state toggle switch
	addParam(ParamWidget::create<NKK>(Vec(150, 200), module, MyModule::TOG2_PARAM, 0.0, 1.0, 1.0));
/* and a lamp it controls */
	addChild(ModuleLightWidget::create<LargeLight<GreenLight>>(Vec(157, 180), module, MyModule::TOG2_LAMP));
//	addChild(ModuleLightWidget::create<RectLED<GreenLight>>(Vec(157, 180), module, MyModule::TOG2_LAMP));

// three state toggle switch
	addParam(ParamWidget::create<NKK>(Vec(185, 200), module, MyModule::TOG3_PARAM, 0.0, 2.0, 1.0));
/* and a lamp it controls the brightness of */
//	addChild(createLight<LargeLight<RedLight>>(Vec(192, 180), module, MyModule::TOG3_LAMP));
		addChild(ModuleLightWidget::create<GiantLED<BlueLight>>(Vec(187, 160), module, MyModule::TOG3_LAMP));

	for (int i = 0; i < 31; i++) {
		addChild(ModuleLightWidget::create<TriadexLight<BlueLight>>(Vec(2, 324 - 7 * i), module, MyModule::TRIADEX_LAMP + i));
//		light[TRIADEX_LAMP].value = 1.0;
	}
	
	for (int i = 31; i < 33; i++) {
		addChild(ModuleLightWidget::create<TriadexLight<YellowLight>>(Vec(2, 324 - 7 * i), module, MyModule::TRIADEX_LAMP + i));
//		light[TRIADEX_LAMP].value = 1.0;
	}

	for (int i = 33; i < 38; i++) {
		addChild(ModuleLightWidget::create<TriadexLight<GreenLight>>(Vec(2, 324 - 7 * i), module, MyModule::TRIADEX_LAMP + i));
//		light[TRIADEX_LAMP].value = 1.0;
	}

	for (int i = 38; i < 40; i++) {
		addChild(ModuleLightWidget::create<TriadexLight<RedLight>>(Vec(2, 324 - 7 * i), module, MyModule::TRIADEX_LAMP + i));
//		light[TRIADEX_LAMP].value = 0.5;
	}

/* illuminatable pushbutton
	addParam(createParam<LEDButton>(Vec(100, 200 + 125), module, MyModule::LED_BUTTON_PARAM, 0.0, 1.0, 0.0));
	addChild(createLight<MediumLight<GreenLight>>(Vec(104, 204 + 125), module, MyModule::LED_BUTTON_LAMP));
 */	

//	addParam(ParamWidget::create<MySnappySlidePot>(Vec(12, 116), module, MyModule::SLIDEPOT_PARAM, 0.0, 20.0, 0.0));
	addParam(ParamWidget::create<MyBSlidePot>(Vec(12, 50), module, MyModule::SLIDEPOT_PARAM, 0.0, 39.0, 0.0));

	addParam(ParamWidget::create<horizontalSlidePot>(Vec(50, 260), module, MyModule::HORIZONTAL_SLIDEPOT_PARAM, 0.0, 39.0, 20.0));

	addParam(ParamWidget::create<BefacoBigSnapKnob>(Vec(75, 150), module, MyModule::STEP12_PARAM, 0.0, 12.0, 0.0));
	addParam(ParamWidget::create<myOwnKnob>(Vec(75 + 17.5, 150 + 17.5), module, MyModule::VER_KNOB_PARAM, -0.5, 0.5, 0.0));

/* 25<-75 get rid of top lampSwitch */
	addParam(ParamWidget::create<LEDButton>(Vec(25 + 28.5, 150 + 28.5), module, MyModule::LED_BUTTON_PARAM, 0.0, 1.0, 0.0));
	addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(25 + 28.5 + 4, 150 + 28.5 + 4), module, MyModule::LED_BUTTON_LAMP));

	addChild(ModuleLightWidget::create<GiantLED<GreenRedLight>>(Vec(25 + 28.5 + 4, 222), module, MyModule::BICOLOR_LAMP));

	addParam(ParamWidget::create<myOwnKnob>(Vec(200, 50), module, MyModule::MYKNOB_PARAM, -INFINITY, +INFINITY, 0.0));

/* try my JWKnob... */
	MyLabelKnob *scaleKnob = dynamic_cast<MyLabelKnob*>(ParamWidget::create<MyLabelKnob>(Vec(100, 275), module, MyModule::LABEL_PARAM, 0.0, 39.0f, 0.0));
	CenteredLabel* const scaleLabel = new CenteredLabel;
	scaleLabel->box.pos = Vec(80, 160);
	scaleLabel->text = "scale here";
	scaleKnob->connectLabel(scaleLabel);
	addChild(scaleLabel);
	addParam(scaleKnob);
	
/* stacked LED buttons idea */
/*
		addParam(ParamWidget::create<LEDBezel>(Vec(200, 300), module, MyModule::BEZEL_PARAM, 0.0, 1.0, 0.0));
		addChild(ModuleLightWidget::create<bigLight<RedLight>>(Vec(202, 302), module, MyModule::BEZEL_LAMP));

		addParam(ParamWidget::create<LEDButton>(Vec(201.5, 301.5), module, MyModule::INSIDE_PARAM, 0.0, 1.0, 0.0));
		addChild(ModuleLightWidget::create<MediumLight<WhiteLight>>(Vec(205.9, 305.9), module, MyModule::INSIDE_LAMP));
*/


		addParam(ParamWidget::create<x8MMBezel>(mm2px(Vec(80 - 12, 110)), module, MyModule::BEZEL_PARAM, 0.0, 1.0, 0.0));
		addChild(ModuleLightWidget::create<x7MMLamp<myRedLight>>(mm2px(Vec(80.5 - 12, 110.5)), module, MyModule::BEZEL_LAMP));

		addParam(ParamWidget::create<x55MMBezel>(mm2px(Vec(81.25 - 12, 111.25)), module, MyModule::INSIDE_PARAM, 0.0, 1.0, 0.0));
		addChild(ModuleLightWidget::create<x5MMLamp<myWhiteLight>>(mm2px(Vec(81.5 - 12, 111.5)), module, MyModule::INSIDE_LAMP));



/* rc/MyModule.cpp:273:37: error: no template named 'GiantLight'; did you mean 'TinyLight'? */

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(Widget::create<ScrewBlack>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


	addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, MyModule::PITCH_PARAM, -3.0, 3.0, 0.0));

	addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, MyModule::PITCH_INPUT));

	addOutput(Port::create<PJ301MPort>(Vec(33, 275), Port::OUTPUT, module, MyModule::SINE_OUTPUT));

	addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, MyModule::BLINK_LIGHT));
	
	addChild(ModuleLightWidget::create<SmallRectLED<whiteRectangularLED>>(mm2px(Vec(60, 111.5)), module, MyModule::TEST_LAMP));
}
};

Model *modelMyModule = Model::create<MyModule, MyModuleWidget>("Playground", "MyModule", "My Module", OSCILLATOR_TAG);
