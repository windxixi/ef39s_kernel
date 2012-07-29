package com.pantech.app.touch;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.util.Log;
import android.view.View;
import java.lang.Integer;
import java.util.Properties;
import java.io.IOException;
import java.io.FileInputStream;

import com.pantech.device.touch.sky_touch;

public class TouchConfig extends Activity  implements OnClickListener {

        private sky_touch mSkyTouch = new sky_touch();

	public static final String TOUCH_CONFIG_FILE= "/data/system/qt602240.cfg";
	public static final int APPLY_TOUCH_CONFIG_CMD= 501;

	private Button buttonLoad, buttonApply;
    
	Properties prop;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);         
		//LinearLayout  
		setContentView(R.layout.touch_screen_config);
		buttonApply = (Button) findViewById(R.id.button_apply);
		buttonLoad = (Button) findViewById(R.id.button_load);
		buttonApply.setOnClickListener(this);
		buttonLoad.setOnClickListener(this);
	}
 
	@Override
	protected void onPause() {
		super.onPause();
	}
 
	@Override
	protected void onResume() {
		super.onResume();
	}

 
	public void onClick(View v) {
		switch(v.getId()) {
			case R.id.button_apply:

				applyPropConfig();
				break;
			case R.id.button_load:
				loadPropFromFile();
				break;
		}
			
	}
	
	public void applyPropConfig()
	{
		if(prop!=null)
		{
			for(int i=0; i<ConfigKeyTable.length; i++)
			{
				String propValue=prop.getProperty(ConfigKeyTable[i].getString());
				
				int propValueInt=-1;
				if(propValue!=null)
				{
					try {
						Integer decodeValue=Integer.decode(propValue.trim());
						propValueInt=decodeValue.intValue();
						
						//propValueInt=Integer.parseInt(propValue.trim());
						//Log.e("cafe2020","propKey:"+TOUCH_CONFIG_KEY_STRING[i]+" propValue:"+propValue+"xxxxxxxxxxxxxxxx\n");
					}catch(Exception e){
						//Log.e("cafe2020","xxxxxxxxxxxxx parseInt failed. propValue="+propValue+"xxxxxxxxxxxxxxxx\n");
					}
					if(propValueInt!=-1)
					{
						setTouchPropConfig(ConfigKeyTable[i].getInt(), propValueInt);
					}
				}
			}
			execTouchCmd(APPLY_TOUCH_CONFIG_CMD,0);
			Toast toast = Toast.makeText(this, "Applying config success.", Toast.LENGTH_SHORT);
			toast.show();
			

		}
		else 
		{
			Toast toast = Toast.makeText(this, "Error. Check config file..", Toast.LENGTH_SHORT);
			toast.show();
		}
	}
	
	public void loadPropFromFile()
	{
		try {
			prop= new Properties();
			FileInputStream stream = new FileInputStream(TOUCH_CONFIG_FILE);
			prop.load(stream);
			stream.close();
			Toast toast = Toast.makeText(this, "Config load success.", Toast.LENGTH_SHORT);
			toast.show();
		} catch (IOException ex) {
			prop = null;
			ex.printStackTrace();
			Toast toast = Toast.makeText(this, "Config load fail.", Toast.LENGTH_SHORT);
			toast.show();
		}
	}
	
	
	public void setTouchPropConfig(int key, int value)
	{
		mSkyTouch.sky_touch_property_set(key, value);
	}
	
	public void execTouchCmd(int cmd, int arg)
	{
		mSkyTouch.sky_touch_cmd(cmd, arg);
	}
	
	
	class ConfigKey {
		private int mIntKey;
		public String mStrKey;
	
		ConfigKey(int intKey, String strKey)
		{
			mIntKey=intKey;
			mStrKey=strKey;
		}
		public String getString()
		{
			return mStrKey;
		}
		public int getInt()
		{
			return mIntKey;
		}
	}
	
		
	
	public ConfigKey[] ConfigKeyTable=
	{
		new ConfigKey(1, 	"POWER_CONFIG_IDLEACQINT"),
		new ConfigKey(2,  	"POWER_CONFIG_ACTVACQINT"),
		new ConfigKey(3,	"POWER_CONFIG_ACTV2IDLETO"),    
		
		new ConfigKey(11,	"ACQUISITION_CONFIG_CHRGTIME"),
		new ConfigKey(12,	"ACQUISITION_CONFIG_ATCHDRIFT"),     
		new ConfigKey(13,	"ACQUISITION_CONFIG_TCHDRIFT"),   
		new ConfigKey(14,	"ACQUISITION_CONFIG_DRIFTST"),   
		new ConfigKey(15,	"ACQUISITION_CONFIG_TCHAUTOCAL"),
		new ConfigKey(16,	"ACQUISITION_CONFIG_SYNC"),
		new ConfigKey(17,	"ACQUISITION_CONFIG_ATCHCALST"),
		new ConfigKey(18, 	"ACQUISITION_CONFIG_ATCHCALSTHR"),
		new ConfigKey(19, 	"ACQUISITION_CONFIG_ATCHFRCCALTHR"),
		new ConfigKey(20, 	"ACQUISITION_CONFIG_ATCHFRCCALRATIO"),

			
		new ConfigKey(31,	"TOUCHSCREEN_CONFIG_CTRL"),
		new ConfigKey(32,	"TOUCHSCREEN_CONFIG_XORIGIN"),
		new ConfigKey(33,	"TOUCHSCREEN_CONFIG_YORIGIN"),  
		new ConfigKey(34,	"TOUCHSCREEN_CONFIG_XSIZE"),
		new ConfigKey(35,	"TOUCHSCREEN_CONFIG_YSIZE"),      
		new ConfigKey(36,	"TOUCHSCREEN_CONFIG_AKSCFG"),     
		new ConfigKey(37,	"TOUCHSCREEN_CONFIG_BLEN"),   
		new ConfigKey(38,	"TOUCHSCREEN_CONFIG_TCHTHR"),    
		new ConfigKey(39,	"TOUCHSCREEN_CONFIG_TCHDI"),     
		new ConfigKey(40,	"TOUCHSCREEN_CONFIG_ORIENT"),    
		new ConfigKey(41,	"TOUCHSCREEN_CONFIG_MRGTIMEOUT"),
		new ConfigKey(42,	"TOUCHSCREEN_CONFIG_MOVHYSTI"),  
		new ConfigKey(43,	"TOUCHSCREEN_CONFIG_MOVHYSTN"),    
		new ConfigKey(44,	"TOUCHSCREEN_CONFIG_MOVFILTER"),    
		new ConfigKey(45,	"TOUCHSCREEN_CONFIG_NUMTOUCH"),     
		new ConfigKey(46,	"TOUCHSCREEN_CONFIG_MRGHYST"),   
		new ConfigKey(47,	"TOUCHSCREEN_CONFIG_MRGTHR"),   
		new ConfigKey(48,	"TOUCHSCREEN_CONFIG_AMPHYST"),    
		new ConfigKey(49,	"TOUCHSCREEN_CONFIG_XRANGE"),   
		new ConfigKey(50,	"TOUCHSCREEN_CONFIG_YRANGE"),    
		new ConfigKey(51,	"TOUCHSCREEN_CONFIG_XLOCLIP"),    
		new ConfigKey(52, 	"TOUCHSCREEN_CONFIG_XHICLIP"),   
		new ConfigKey(53,	"TOUCHSCREEN_CONFIG_YLOCLIP"),   
		new ConfigKey(54,	"TOUCHSCREEN_CONFIG_YHICLIP"),   
		new ConfigKey(55, 	"TOUCHSCREEN_CONFIG_XEDGECTRL"),  
		new ConfigKey(56, 	"TOUCHSCREEN_CONFIG_XEDGEDIST"), 
		new ConfigKey(57, 	"TOUCHSCREEN_CONFIG_YEDGECTRL"), 
		new ConfigKey(58, 	"TOUCHSCREEN_CONFIG_YEDGEDIST"), 
		new ConfigKey(59, 	"TOUCHSCREEN_CONFIG_JUMPLIMIT"), 
		new ConfigKey(60, 	"TOUCHSCREEN_CONFIG_TCHHYST"),
		new ConfigKey(61, 	"TOUCHSCREEN_CONFIG_XPITCH"),
		new ConfigKey(62, 	"TOUCHSCREEN_CONFIG_YPITCH"),
		new ConfigKey(63, 	"TOUCHSCREEN_CONFIG_NEXTTCHDI"),
		
		new ConfigKey(71, 	"KEYARRAY_CONFIG_CTRL"),     
		new ConfigKey(72,	"KEYARRAY_CONFIG_XORIGIN"),
		new ConfigKey(73,	"KEYARRAY_CONFIG_YORIGIN"), 
		new ConfigKey(74,	"KEYARRAY_CONFIG_XSIZE"), 
		new ConfigKey(75, 	"KEYARRAY_CONFIG_YSIZE"),   
		new ConfigKey(76, 	"KEYARRAY_CONFIG_AKSCFG"),   
		new ConfigKey(77, 	"KEYARRAY_CONFIG_BLEN"),  
		new ConfigKey(78, 	"KEYARRAY_CONFIG_TCHTHR"),   
		new ConfigKey(79, 	"KEYARRAY_CONFIG_TCHDI"),  
				
		new ConfigKey(91, 	"COMC_CONFIG_CTRL"),     
		new ConfigKey(92, 	"COMC_CONFIG_COMMAND"),
				
		new ConfigKey(101, 	"GPIOPWM_CONFIG_CTRL"), 
		new ConfigKey(102, 	"GPIOPWM_CONFIG_REPORTMASK"),
		new ConfigKey(103, 	"GPIOPWM_CONFIG_DIR"),
		new ConfigKey(104, 	"GPIOPWM_CONFIG_INTPULLUP"),
		new ConfigKey(105, 	"GPIOPWM_CONFIG_OUT"),
		new ConfigKey(106, 	"GPIOPWM_CONFIG_WAKE"),    
		new ConfigKey(107, 	"GPIOPWM_CONFIG_PWM"),   
		new ConfigKey(108, 	"GPIOPWM_CONFIG_PERIOD"),
		new ConfigKey(109, 	"GPIOPWM_CONFIG_DUTY0"), 
		new ConfigKey(110, 	"GPIOPWM_CONFIG_DUTY1"),  
		new ConfigKey(111, 	"GPIOPWM_CONFIG_DUTY2"),  
		new ConfigKey(112, 	"GPIOPWM_CONFIG_DUTY3"),  
		new ConfigKey(113, 	"GPIOPWM_CONFIG_TRIGGER0"),
		new ConfigKey(114, 	"GPIOPWM_CONFIG_TRIGGER1"),
		new ConfigKey(115, 	"GPIOPWM_CONFIG_TRIGGER2"),
		new ConfigKey(116, 	"GPIOPWM_CONFIG_TRIGGER3"),
				

	
				
		new ConfigKey(241, 	"SELFTEST_CONFIG_CTRL"),     
		new ConfigKey(242, 	"SELFTEST_CONFIG_CMD"),
		
		new ConfigKey(131, 	"GRIPFACESUPPRESSION_CONFIG_CTRL"),
		new ConfigKey(132, 	"GRIPFACESUPPRESSION_CONFIG_XLOGRIP"),
		new ConfigKey(133, 	"GRIPFACESUPPRESSION_CONFIG_XHIGRIP"),
		new ConfigKey(134, 	"GRIPFACESUPPRESSION_CONFIG_YLOGRIP"),
		new ConfigKey(135, 	"GRIPFACESUPPRESSION_CONFIG_YHIGRIP"),
		
	
		new ConfigKey(231, 	"TOUCHSUPPRESSION_CTRL"),
		new ConfigKey(232, 	"TOUCHSUPPRESSION_APPRTHR"),
		new ConfigKey(233, 	"TOUCHSUPPRESSION_MAXAPPRAREA"),
		new ConfigKey(234, 	"TOUCHSUPPRESSION_MAXTCHAREA"),
		new ConfigKey(235, 	"TOUCHSUPPRESSION_SUPSTRENGTH"),
		new ConfigKey(236, 	"TOUCHSUPPRESSION_SUPEXTTO"),
		new ConfigKey(237, 	"TOUCHSUPPRESSION_MAXNUMTCHS"),
		new ConfigKey(238, 	"TOUCHSUPPRESSION_SHAPESTRENGTH"),

		
		new ConfigKey(271, 	"CTE_CONFIG_CTRL"),     
		new ConfigKey(272, 	"CTE_CONFIG_MODE"),     
		new ConfigKey(273, 	"CTE_CONFIG_IDLEGCAFDEPTH"),     
		new ConfigKey(274, 	"CTE_CONFIG_ACTVGCAFDEPTH"),     
		new ConfigKey(275, 	"CTE_CONFIG_ADCSPERSYNC"),     
		new ConfigKey(276, 	"CTE_CONFIG_PULSESPERADC"),
		new ConfigKey(277, 	"CTE_CONFIG_XSLEW"),
		new ConfigKey(278, 	"CTE_CONFIG_SYNCDELAY"),
		
		new ConfigKey(281, 	"STYLUS_CTRL"),
		new ConfigKey(282, 	"STYLUS_CONTMIN"),
		new ConfigKey(283, 	"STYLUS_CONTMAX"),
		new ConfigKey(284, 	"STYLUS_STABILITY"),
		new ConfigKey(285, 	"STYLUS_MAXTCHAREA"),
		new ConfigKey(286, 	"STYLUS_AMPLTHR"),
		new ConfigKey(287, 	"STYLUS_STYSHAPE"),
		new ConfigKey(288, 	"STYLUS_HOVERSUP"),
		new ConfigKey(289, 	"STYLUS_CONFTHR"),
		new ConfigKey(290, 	"STYLUS_SYNCSPERX"),

		
		new ConfigKey(161, 	"NOISESUPPRESSION_CTRL"),
		new ConfigKey(162, 	"NOISESUPPRESSION_CFG"),
		new ConfigKey(163, 	"NOISESUPPRESSION_CALCFG"),
		new ConfigKey(164, 	"NOISESUPPRESSION_BASEFREQ"),
		new ConfigKey(165, 	"NOISESUPPRESSION_MFFREQ0"),
		new ConfigKey(166, 	"NOISESUPPRESSION_MFFREQ1"),
		new ConfigKey(167, 	"NOISESUPPRESSION_GCACTVINVLDADCS"),
		new ConfigKey(168, 	"NOISESUPPRESSION_GCIDLEINVLDADCS"),
		new ConfigKey(169, 	"NOISESUPPRESSION_GCMAXADCSPERX"),
		new ConfigKey(170, 	"NOISESUPPRESSION_GCLIMITMIN"),
		new ConfigKey(171, 	"NOISESUPPRESSION_GCLIMITMAX"),
		new ConfigKey(172, 	"NOISESUPPRESSION_GCCOUNTMINTGT"),
		new ConfigKey(173, 	"NOISESUPPRESSION_MFINVLDDIFFTHR"),
		new ConfigKey(174, 	"NOISESUPPRESSION_MFINCADCSPXTHR"),
		new ConfigKey(175, 	"NOISESUPPRESSION_MFERRORTHR"),
		new ConfigKey(176, 	"NOISESUPPRESSION_SELFREQMAX"),
		new ConfigKey(177, 	"NOISESUPPRESSION_BLEN"),
		new ConfigKey(178, 	"NOISESUPPRESSION_TCHTHR"),
		new ConfigKey(179, 	"NOISESUPPRESSION_TCHDI"),
		new ConfigKey(180, 	"NOISESUPPRESSION_MOVHYSTI"),
		new ConfigKey(181, 	"NOISESUPPRESSION_MOVHYSTN"),
		new ConfigKey(182, 	"NOISESUPPRESSION_MOVFILTER"),
		new ConfigKey(183, 	"NOISESUPPRESSION_NUMTOUCH"),
		new ConfigKey(184, 	"NOISESUPPRESSION_MRGHYST"),
		new ConfigKey(185, 	"NOISESUPPRESSION_MRGTHR"),
		new ConfigKey(186, 	"NOISESUPPRESSION_XLOCLIP"),
		new ConfigKey(187, 	"NOISESUPPRESSION_XHICLIP"),
		new ConfigKey(188, 	"NOISESUPPRESSION_YLOCLIP"),
		new ConfigKey(189, 	"NOISESUPPRESSION_YHICLIP"),
		new ConfigKey(190, 	"NOISESUPPRESSION_XEDGECTRL"),
		new ConfigKey(191, 	"NOISESUPPRESSION_XEDGEDIST"),
		new ConfigKey(192, 	"NOISESUPPRESSION_YEDGECTRL"),
		new ConfigKey(193, 	"NOISESUPPRESSION_YEDGEDIST"),
		new ConfigKey(194, 	"NOISESUPPRESSION_JUMPLIMIT"),
		new ConfigKey(195, 	"NOISESUPPRESSION_TCHHYST"),
		new ConfigKey(196, 	"NOISESUPPRESSION_NEXTTCHDI"),
		
	};
	
}
