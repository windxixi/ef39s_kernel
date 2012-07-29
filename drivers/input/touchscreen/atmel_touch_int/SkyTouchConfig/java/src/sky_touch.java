/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


package com.pantech.device.touch;


/**
 * Provides access to Sky_touch API.
 */
public class sky_touch {
  private static final String TAG="sky_touch";

  static {
    System.loadLibrary("sky_touch");
  }

  public sky_touch(){
  }


  public void sky_touch_property_set(int key, int value)
  {
  	sky_touch_ioctl_java(key, value);
  }
  
  public int sky_touch_cmd(int cmd, int arg)
  {
  	return sky_touch_ioctl_java(cmd, arg);
  }
  
  public int sky_touch_restore()
  {
  	return sky_touch_mode_restore_java();
  }

  public int sky_touch_set(int enable)
  {
  	return sky_touch_mode_set_java(enable);
  }

  public int sky_touch_get()
  {
  	return sky_touch_mode_get_java();
  }
  
  private static native int sky_touch_ioctl_java(int cmd, int arg);   
  private static native int sky_touch_mode_restore_java();
  private static native int sky_touch_mode_set_java(int enable);
  private static native int sky_touch_mode_get_java();
}
