package de.in.tum.campar;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class ShowExchangeMenu extends Activity {
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.showexchangemenu);
		
		Log.d("ShowMarker",	"show exchange menu");

	}
}
