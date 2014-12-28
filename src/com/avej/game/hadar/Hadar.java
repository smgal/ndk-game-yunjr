
package com.avej.game.hadar;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.media.MediaPlayer;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

class GameConfig
{
	final static boolean USE_LOG_LOOP = false;
	final static boolean USE_LOG_LIFE_CICLE = false;
	final static boolean USE_BGM = false;

	// 1280x720 and 6400x360 will be supported finally 
	final static int BUFFER_WIDTH = 720;
	final static int BUFFER_HEIGHT = 1280;

	static boolean is_terminating = false;
	// Atomic operation is guaranteed.
	static boolean is_rendering_queue_empty = true;

	static boolean just_touch = false;
	static int    touch_x = -1;
	static int    touch_y = -1;

	static int    prev_canvas_w = BUFFER_WIDTH;
	static int    prev_canvas_h = BUFFER_HEIGHT;

	static Bitmap src_bitmap;
	static Rect   src_rect = new Rect(0, 0, GameConfig.BUFFER_WIDTH, GameConfig.BUFFER_HEIGHT);
	static Rect   dst_rect = new Rect();
	static double scaling_factor_x = 1.0;
	static double scaling_factor_y = 1.0;
	static long   start_time = 0;

	static GameTask game_task;
	static MediaPlayer media_player;

	public static void processTouchEvent()
	{
		if (!GameConfig.just_touch)
			return;

		int result = 0;
		
		if (GameConfig.touch_x >= 0 && GameConfig.touch_y >= 0)
		{
			int revised_touch_x = GameConfig.touch_x;
			int revised_touch_y = GameConfig.touch_y;

			if (revised_touch_x >= 0 && revised_touch_y >= 0 )
			{
				revised_touch_x -= GameConfig.dst_rect.left;
				revised_touch_y -= GameConfig.dst_rect.top;
				
				revised_touch_x = (int)((double)revised_touch_x * GameConfig.scaling_factor_x);
				revised_touch_y = (int)((double)revised_touch_y * GameConfig.scaling_factor_y);
			}
			
			result = YozoraView.processYozora(System.currentTimeMillis() - GameConfig.start_time, revised_touch_x, revised_touch_y);
		}
		else
		{
			result = YozoraView.processYozora(System.currentTimeMillis() - GameConfig.start_time, GameConfig.touch_x, GameConfig.touch_y);
		}
		
		GameConfig.just_touch = false;

		if (result == 0)
			GameConfig.is_terminating = true;
	}
}

public class Hadar extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		if (GameConfig.USE_LOG_LIFE_CICLE)
			android.util.Log.i("[SMGAL]", "[MAIN] activity in onCreate()");
		
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		requestWindowFeature(Window.FEATURE_NO_TITLE);

		if (GameConfig.BUFFER_WIDTH > GameConfig.BUFFER_HEIGHT)
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		else
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		
		GameConfig.is_terminating = false;

		GameConfig.just_touch = false;
		GameConfig.touch_x = -1;
		GameConfig.touch_y = -1;
		
		YozoraView yozoraView = new YozoraView(this);
		setContentView(yozoraView);
		
		GameConfig.game_task = new GameTask(this, yozoraView);
		GameConfig.game_task.execute();
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		
		if (GameConfig.USE_LOG_LIFE_CICLE)
			android.util.Log.i("[SMGAL]", "[MAIN] activity in onDestroy()");
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
		
		if (GameConfig.USE_LOG_LIFE_CICLE)
			android.util.Log.i("[SMGAL]", "[MAIN] activity in onResume()");
		
		if (GameConfig.USE_BGM)
		{
			// temporary play for test
			GameConfig.media_player = MediaPlayer.create(Hadar.this, R.raw.lore_1);
			GameConfig.media_player.setLooping(true);
			GameConfig.media_player.start();
		}
	}

	@Override
	protected void onPause()
	{
		super.onPause();
		
		if (GameConfig.USE_LOG_LIFE_CICLE)
			android.util.Log.i("[SMGAL]", "[MAIN] activity in onPause()");
		
		if (GameConfig.USE_BGM)
		{
			GameConfig.media_player.stop();
			GameConfig.media_player.release();
		}
	}

	@Override
	public void onBackPressed()
	{
		// Back key will not process in this method 
		// super.onBackPressed();
		GameConfig.is_terminating = true;
	}

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		int motion_event = event.getAction(); 

		if (motion_event == MotionEvent.ACTION_DOWN)
		{
			int ax = (int)event.getX();
			int ay = (int)event.getY();

			GameConfig.touch_x = ax;
			GameConfig.touch_y = ay;
			GameConfig.just_touch = true;
		}
		else if (motion_event == MotionEvent.ACTION_MOVE)
		{
			int ax = (int)event.getX();
			int ay = (int)event.getY();

			GameConfig.touch_x = ax;
			GameConfig.touch_y = ay;
			GameConfig.just_touch = true;
		}
		else if (motion_event == MotionEvent.ACTION_UP)
		{
			GameConfig.touch_x = -1;
			GameConfig.touch_y = -1;
			GameConfig.just_touch = true;
		}

		return true;
	}

    /* load native library */
	static
	{
		System.loadLibrary("hadar");
	}
}

class YozoraView extends View
{
	// These functions are implemented in libhadar.so
	public static native void initYozora(String resource_path, String app_name);
	public static native void doneYozora();
	public static native int  processYozora(long time_ms, int motion_x, int motion_y);
	public static native int  renderYozora(Bitmap bitmap);

	public YozoraView(Context context)           
	{
		super(context);
		
		{
			DisplayMetrics metrics = new DisplayMetrics();
			((Hadar)context).getWindowManager().getDefaultDisplay().getMetrics(metrics);

			android.util.Log.i("[SMGAL]", "[MAIN] Screen size (" + metrics.widthPixels + "x" + metrics.heightPixels + ")");
		}

		GameConfig.src_bitmap = Bitmap.createBitmap(GameConfig.BUFFER_WIDTH, GameConfig.BUFFER_HEIGHT, Bitmap.Config.ARGB_8888);

		GameConfig.start_time = System.currentTimeMillis();
	}

	private static int saved_canvas_w = 0;
	private static int saved_canvas_h = 0;
	
	@Override protected void onDraw(Canvas canvas)
	{
		if (saved_canvas_w != GameConfig.prev_canvas_w || saved_canvas_h != GameConfig.prev_canvas_h)
		{
			saved_canvas_w = GameConfig.prev_canvas_w;
			saved_canvas_h = GameConfig.prev_canvas_h;

			if (GameConfig.src_rect.width() * saved_canvas_h > GameConfig.src_rect.height() * saved_canvas_w)
			{
				int dst_width = saved_canvas_w;
				int dst_height = GameConfig.src_rect.height() * saved_canvas_w / GameConfig.src_rect.width();
				int dst_x = (saved_canvas_w - dst_width) / 2;
				int dst_y = (saved_canvas_h - dst_height) / 2;
				
				GameConfig.dst_rect.set(dst_x, dst_y, dst_x + dst_width, dst_y + dst_height);
			}
			else
			{
				int dst_width = GameConfig.src_rect.width() * saved_canvas_h / GameConfig.src_rect.height();
				int dst_height = saved_canvas_h;
				int dst_x = (saved_canvas_w - dst_width) / 2;
				int dst_y = (saved_canvas_h - dst_height) / 2;

				GameConfig.dst_rect.set(dst_x, dst_y, dst_x + dst_width, dst_y + dst_height);
			}

			GameConfig.scaling_factor_x = 1.0 * GameConfig.src_rect.width() / GameConfig.dst_rect.width();
			GameConfig.scaling_factor_y = 1.0 * GameConfig.src_rect.height() / GameConfig.dst_rect.height();
		}
		
		GameConfig.prev_canvas_w = canvas.getWidth();
		GameConfig.prev_canvas_h = canvas.getHeight();
		
		// [LOCK] GameConfig.src_bitmap
		{
			canvas.drawBitmap(GameConfig.src_bitmap, GameConfig.src_rect, GameConfig.dst_rect, null);
			GameConfig.is_rendering_queue_empty = true;
		}
		// [UNLOCK] GameConfig.src_bitmap
	}
}

class GameTask extends AsyncTask<Void, Void, Void>
{
	private Activity activity;
	private View view;

	private static long base_tick;

	GameTask(Activity activity, View view)
	{
		this.activity = activity;
		this.view = view;
		
		base_tick = System.currentTimeMillis();
	}
	
	@Override
	protected Void doInBackground(Void... params)
	{
		PackageInfo info = null;

		try
		{
			info = view.getContext().getPackageManager().getPackageInfo("com.avej.game.hadar", 0);
		}
		catch (NameNotFoundException e)
		{
			Log.e("[SMGAL]", e.toString());
			return null;
		}
		
		YozoraView.initYozora(info.applicationInfo.sourceDir, "hadar");
		
		while (!GameConfig.is_terminating)
		{
			while (!GameConfig.is_rendering_queue_empty)
			{
				try
				{
					Thread.sleep(0);
				}
				catch (InterruptedException e)
				{
					e.printStackTrace();
				}
			}
			
			// [LOCK] GameConfig.src_bitmap
			int result = YozoraView.renderYozora(GameConfig.src_bitmap); 
			// [UNLOCK] GameConfig.src_bitmap
	
			if (result == 0)
				break;

			this.publishProgress();
			
			try
			{
				Thread.sleep(20);
			}
			catch (InterruptedException e)
			{
				e.printStackTrace();
			}

			if (GameConfig.USE_LOG_LOOP)
				android.util.Log.i("[SMGAL]", "[WORKER] in game loop");
		}
		
		YozoraView.doneYozora();
		
		return null;
	}

	@Override
	protected void onProgressUpdate(Void... values)
	{
		assert(GameConfig.is_rendering_queue_empty);
		
		if (GameConfig.USE_LOG_LOOP)
			android.util.Log.i("[SMGAL]", "[MAIN] screen update");

		GameConfig.processTouchEvent();
		
		GameConfig.is_rendering_queue_empty = false;
		view.invalidate();
	}

	@Override
	protected void onPostExecute(Void result)
	{
		if (GameConfig.USE_LOG_LIFE_CICLE)
			android.util.Log.i("[SMGAL]", "[MAIN] activity just before calling finish()");
		
		this.activity.finish();
		//??System.exit(0);
	}

	@SuppressWarnings("unused")
	private static int getTicks()
	{
		long tick = System.currentTimeMillis() - base_tick;
		return (int)tick;
	}
	
	@SuppressWarnings("unused")
	private static void waitMillisecond(int msec)
	{
		try
		{
			Thread.sleep(msec);
		}
		catch (InterruptedException e)
		{
			e.printStackTrace();
		}
	}

	@SuppressWarnings("unused")
	private static void updateScreen()
	{
		if (GameConfig.USE_LOG_LIFE_CICLE)
			android.util.Log.i("[SMGAL]", "updateScreen() calling...");

		GameConfig.is_rendering_queue_empty = false;

		GameConfig.game_task.publishProgress();

		while (!GameConfig.is_rendering_queue_empty)
		{
			try
			{
				Thread.sleep(0);
			}
			catch (InterruptedException e)
			{
				e.printStackTrace();
			}
		}
	}

}
