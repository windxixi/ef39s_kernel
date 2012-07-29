How to Build
    1. Get Toolchain (arm-eabi-4.4.0) and install
       (Visit http://www.codesourcery.com/sgpp/portal/release1033 or http://android.git.kernel.org/?p=platform/prebuilt.git)   

    2. run build_kernel.sh
		
      $ export ARCH=arm
      $ export CROSS_COMPILE=~/your toolchain path/arm-eabi-4.4.0/bin/arm-eabi-
      $ ./build_kernel.sh

    3.Output Files
      -	kernel : ef39s_kernel/obj/KERNEL_OBJ/arch/arm/boot/zImage
      -	module : ef39s_kernel/obj/KERNEL_OBJ/drivers/*/*.ko
 
   
